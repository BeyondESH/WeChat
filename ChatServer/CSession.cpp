#include "LogicSystem.h"
#include "CSession.h"
#include <boost/uuid.hpp>
#include "LogicNode.h"
#include "CServer.h"
#include "SendNode.h"
#include "SessionMgr.h"
#include "StatusGrpcClient.h"
#include "MySQLMgr.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

CSession::CSession(boost::asio::io_context &ioc, std::shared_ptr<CServer> cserver) 
    : _cserver(cserver), _socket(ioc), _isStop(false), _msgId(0), _msgLen(0) {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    _sessionId = boost::uuids::to_string(uuid);
    _lastActiveTime = std::chrono::steady_clock::now();
}

void CSession::start() {
    auto self = shared_from_this();
    readHead();
}

boost::asio::ip::tcp::socket &CSession::getSocket() {
    return this->_socket;
}

std::string &CSession::getSessionId() {
    return _sessionId;
}

void CSession::readHead() {
    auto self = shared_from_this();
    boost::asio::async_read(
        _socket,
        boost::asio::buffer(_headBuf, MSG_HEAD_SIZE),
        [self](boost::system::error_code ec, std::size_t /*bytes*/) {
            if (ec) {
                std::cerr << "read head error: " << ec.what() << std::endl;
                if (ec == boost::asio::error::eof ||
                    ec == boost::asio::error::connection_reset) {
                    if (self->_isAuthenticated && self->_uid > 0)
                        SessionMgr::getInstance()->removeSession(self->_uid);
                    self->_cserver->closeSession(self->getSessionId());
                }
                return;
            }

            short id, len;
            memcpy(&id, self->_headBuf, 2);
            memcpy(&len, self->_headBuf + 2, 2);
            self->_msgId = ntohs(id);
            self->_msgLen = ntohs(len);

            if (self->_msgLen < 0 || self->_msgLen > MSG_MAX_LEN) {
                std::cerr << "invalid body length: " << self->_msgLen << std::endl;
                self->_cserver->closeSession(self->getSessionId());
                return;
            }

            self->_lastActiveTime = std::chrono::steady_clock::now();

            if (self->_msgLen == 0) {
                self->handleMessage(self->_msgId, "");
                self->readHead();
                return;
            }

            self->readBody();
        });
}

void CSession::readBody() {
    auto self = shared_from_this();
    _bodyBuf.resize(_msgLen);
    boost::asio::async_read(
        _socket,
        boost::asio::buffer(_bodyBuf, _msgLen),
        [self](boost::system::error_code ec, std::size_t /*bytes*/) {
            if (ec) {
                std::cerr << "read body error: " << ec.what() << std::endl;
                if (self->_isAuthenticated && self->_uid > 0)
                    SessionMgr::getInstance()->removeSession(self->_uid);
                self->_cserver->closeSession(self->getSessionId());
                return;
            }

            std::string msg_str(self->_bodyBuf.data(), self->_msgLen);
            self->handleMessage(self->_msgId, msg_str);

            self->readHead();
        });
}

void CSession::handleMessage(short msgId, const std::string &msgBody) {
    auto self = shared_from_this();

    if (msgId == ID_HEARTBEAT_PING) {
        json response;
        response["status"] = "alive";
        std::string resp_str = response.dump();
        self->send(const_cast<char*>(resp_str.c_str()), resp_str.length(), ID_HEARTBEAT_PONG);
        return;
    }

    if (msgId == ID_CHAT_LOGIN) {
        try {
            json data = json::parse(msgBody);
            if (data.contains("uid") && data.contains("token")) {
                int uid = data["uid"].get<int>();
                std::string token = data["token"].get<std::string>();
                
                auto respond = StatusGrpcClient::getInstance()->checkToken(uid, token);
                if (respond.error() == ErrorCodes::SUCCESS) {
                    self->_isAuthenticated = true;
                    self->_uid = uid;
                    
                    SessionMgr::getInstance()->addSession(uid, self);
                    
                    std::string channel = "user:" + std::to_string(uid);
                    RedisMgr::getInstance()->subscribe(channel);
                    
                    RedisMgr::getInstance()->hSet("online_users", std::to_string(uid), "ChatServer");
                    RedisMgr::getInstance()->hIncrBy("chatserver:ChatServer:connections", "count", 1);
                    
                    json response;
                    response["error"] = ErrorCodes::SUCCESS;
                    response["token"] = token;
                    std::string resp_str = response.dump();
                    self->send(const_cast<char*>(resp_str.c_str()), resp_str.length(), ID_CHAT_LOGIN_RSP);
                    
                    self->sendInitialData();
                } else {
                    json response;
                    response["error"] = respond.error();
                    std::string resp_str = response.dump();
                    self->send(const_cast<char*>(resp_str.c_str()), resp_str.length(), ID_CHAT_LOGIN_RSP);
                }
            }
        } catch (std::exception& e) {
            std::cerr << "解析登录消息异常: " << e.what() << std::endl;
            json response;
            response["error"] = ErrorCodes::ERROR_JSON;
            std::string resp_str = response.dump();
            self->send(const_cast<char*>(resp_str.c_str()), resp_str.length(), ID_CHAT_LOGIN_RSP);
        }
        return;
    }

    if (!self->_isAuthenticated) {
        std::cerr << "用户未认证，拒绝处理消息 ID: " << msgId << std::endl;
        json response;
        response["error"] = ErrorCodes::Token_Invalid;
        std::string resp_str = response.dump();
        self->send(const_cast<char*>(resp_str.c_str()), resp_str.length(), msgId + 1);
        return;
    }

    std::cout << "接收到消息 ID:" << msgId << std::endl;
    
    std::shared_ptr<LogicNode> logicNode = std::make_shared<LogicNode>(msgId, msgBody, self);
    LogicSystem::getInstance()->postMsgToQueue(logicNode);
}

void CSession::send(char *data, short totalLen, short id) {
    try {
        auto self = shared_from_this();
        std::lock_guard<std::mutex> lock(_mutex);
        int queueSize = _send_queue.size();
        if (queueSize > QUEUE_MAX_SIZE) {
            std::cerr << "queue size is overflow:" << queueSize << std::endl;
            return;
        }

        char *msgData = new char[totalLen + 4];
        short network_id = htons(id);
        short network_len = htons(totalLen);
        memcpy(msgData, &network_id, 2);
        memcpy(msgData + 2, &network_len, 2);
        memcpy(msgData + 4, data, totalLen);
        _send_queue.emplace(std::make_shared<SendNode>(msgData, id, totalLen + 4));

        if (queueSize > 0) {
            return;
        }
        auto &sendNode = _send_queue.front();
        boost::asio::async_write(_socket, boost::asio::buffer(sendNode->_data, sendNode->_totalLen),
                                 std::bind(&CSession::handle_async_write, self, std::placeholders::_1,
                                           std::placeholders::_2, self));
    } catch (const boost::system::error_code &ec) {
        std::cerr << "send error:" << ec.what() << std::endl;
        close();
        _cserver->closeSession(getSessionId());
    }
}

void CSession::close() {
    _socket.close();
    _isStop = true;
    
    if (_isAuthenticated && _uid > 0) {
        std::string channel = "user:" + std::to_string(_uid);
        RedisMgr::getInstance()->unsubscribe(channel);
        
        RedisMgr::getInstance()->hDel("online_users", std::to_string(_uid));
        RedisMgr::getInstance()->hDecrBy("chatserver:ChatServer:connections", "count", 1);
    }
}

void CSession::handle_async_write(const boost::system::error_code &ec, std::size_t bytes_transferred,
                                  std::shared_ptr<CSession> self) {
    try {
        std::lock_guard<std::mutex> lock(self->_mutex);
        if (ec.value() != 0) {
            std::cerr << "send error:" << ec.what() << std::endl;
            self->close();
            self->_cserver->closeSession(self->getSessionId());
            return;
        }
        self->_send_queue.pop();
        if (!self->_send_queue.empty()) {
            auto &sendNode = self->_send_queue.front();
            boost::asio::async_write(self->_socket, boost::asio::buffer(sendNode->_data, sendNode->_totalLen),
                                     std::bind(&CSession::handle_async_write, self, std::placeholders::_1,
                                               std::placeholders::_2, self));
        }
    } catch (const boost::system::error_code &ec) {
        std::cerr << "handle async write error:" << ec.what() << std::endl;
        self->close();
        self->_cserver->closeSession(self->getSessionId());
    }
}

void CSession::sendInitialData() {
    if (!_isAuthenticated || _uid <= 0) {
        return;
    }
    
    try {
        auto friends = MySQLMgr::getInstance()->getFriendList(_uid);
        json friendsJson = json::array();
        
        for (const auto& f : friends) {
            json userJson;
            userJson["uid"] = f.uid;
            userJson["name"] = f.name;
            userJson["avatar"] = f.avatar;
            userJson["status"] = f.status;
            userJson["isOnline"] = SessionMgr::getInstance()->isUserOnline(f.uid);
            friendsJson.push_back(userJson);
        }
        
        json response;
        response["error"] = ErrorCodes::SUCCESS;
        response["friends"] = friendsJson;
        std::string resp_str = response.dump();
        send(const_cast<char*>(resp_str.c_str()), resp_str.length(), ID_FRIEND_LIST_RSP);
        
        auto requests = MySQLMgr::getInstance()->getFriendRequests(_uid);
        json requestsJson = json::array();
        
        for (const auto& req : requests) {
            json reqJson;
            reqJson["requestId"] = req.first;
            reqJson["fromUid"] = req.second.uid;
            reqJson["fromName"] = req.second.name;
            reqJson["fromAvatar"] = req.second.avatar;
            requestsJson.push_back(reqJson);
        }
        
        if (!requestsJson.empty()) {
            json reqResponse;
            reqResponse["error"] = ErrorCodes::SUCCESS;
            reqResponse["requests"] = requestsJson;
            std::string req_str = reqResponse.dump();
            send(const_cast<char*>(req_str.c_str()), req_str.length(), ID_FRIEND_REQUEST_LIST_RSP);
        }
        
        auto messages = MySQLMgr::getInstance()->getLastMessages(_uid, 20);
        json messagesJson = json::array();
        
        for (const auto& msg : messages) {
            json msgJson;
            int contactId = std::stoi(msg.at("contact_id"));
            UserInfo contact = MySQLMgr::getInstance()->getUserById(contactId);
            
            msgJson["contactUid"] = contactId;
            msgJson["contactName"] = contact.name;
            msgJson["contactAvatar"] = contact.avatar;
            msgJson["lastMessage"] = msg.at("content");
            msgJson["lastTime"] = msg.at("send_time");
            msgJson["isSelf"] = (std::stoi(msg.at("from_uid")) == _uid);
            
            messagesJson.push_back(msgJson);
        }
        
        if (!messagesJson.empty()) {
            json msgResponse;
            msgResponse["error"] = ErrorCodes::SUCCESS;
            msgResponse["recentMessages"] = messagesJson;
            std::string msg_str = msgResponse.dump();
            send(const_cast<char*>(msg_str.c_str()), msg_str.length(), ID_RECENT_MESSAGES_RSP);
        }
    } catch (std::exception& e) {
        std::cerr << "发送初始数据时出错: " << e.what() << std::endl;
    }
}