//
// Created by Beyond on 2024/11/3.
//
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

CSession::CSession(boost::asio::io_context &ioc, std::shared_ptr<CServer> cserver) : _cserver(cserver), _socket(ioc),
    _isStop(false), _isParsed(false), _msg_head(std::make_shared<MsgNode>(0, 4)),
    _msg_body(std::make_shared<MsgNode>(0, 0)) {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    _sessionId = boost::uuids::to_string(uuid);
}

void CSession::start() {
    auto self = shared_from_this();
    read();
}

boost::asio::ip::tcp::socket &CSession::getSocket() {
    return this->_socket;
}

std::string &CSession::getSessionId() {
    return _sessionId;
}

void CSession::read() {
    auto self = shared_from_this();
    // 读取并处理请求
    _socket.async_read_some(boost::asio::buffer(_buffer, MSG_MAX_LEN),
                            [self](boost::system::error_code ec, std::size_t bytes_transferred) {
                                try {
                                    if (ec.value() != 0) {
                                        std::cerr << "read error:" << ec.what() << std::endl;
                                        if (ec == boost::asio::error::eof || ec ==
                                            boost::asio::error::connection_reset) {
                                            // 连接已关闭或重置，从会话管理器中移除
                                            if (self->_isAuthenticated && self->_uid > 0) {
                                                SessionMgr::getInstance()->removeSession(self->_uid);
                                            }
                                            self->_cserver->closeSession(self->getSessionId());
                                            return;
                                        }
                                        self->read();
                                        return;
                                    }

                                    if (bytes_transferred == 0) {
                                        self->read();
                                        return;
                                    }

                                    // 记录当前处理的缓冲区位置
                                    size_t buffer_pos = 0;

                                    while (buffer_pos < bytes_transferred) {
                                        //读取头部
                                        if (self->_isParsed == false) {
                                            // 计算还需要读取多少字节的头部数据
                                            size_t head_remaining = MSG_HEAD_SIZE - self->_msg_head->currentLen;
                                            // 计算本次可以读取的头部字节数
                                            size_t head_to_copy = std::min(
                                                head_remaining, bytes_transferred - buffer_pos);

                                            // 从正确的缓冲区位置拷贝数据
                                            memcpy(self->_msg_head->data + self->_msg_head->currentLen,
                                                   self->_buffer + buffer_pos,
                                                   head_to_copy);

                                            self->_msg_head->currentLen += head_to_copy;
                                            buffer_pos += head_to_copy;

                                            // 如果头部还没读取完整，等待下次读取
                                            if (self->_msg_head->currentLen < MSG_HEAD_SIZE) {
                                                self->read();
                                                return;
                                            }

                                            //解析头部信息,获取body长度和消息ID
                                            short id;
                                            memcpy(&id, self->_msg_head->data, 2);
                                            id = boost::asio::detail::socket_ops::network_to_host_short(id);
                                            self->_msgId = id;

                                            memcpy(&self->_msg_body->totalLen, self->_msg_head->data + 2, 2);
                                            self->_msg_body->totalLen =
                                                    boost::asio::detail::socket_ops::network_to_host_short(
                                                        self->_msg_body->totalLen);
                                                        
                                            if (self->_msg_body->totalLen > MSG_MAX_LEN) {
                                                std::cerr << "msg body length is invalid:" << self->_msg_body->totalLen
                                                        << std::endl;
                                                self->_cserver->closeSession(self->getSessionId());
                                                return;
                                            }
                                            self->_msg_body = std::make_shared<MsgNode>(0, self->_msg_body->totalLen);
                                            self->_isParsed = true;
                                        }

                                        //读取body
                                        if (self->_isParsed) {
                                            // 计算还需要读取多少字节的消息体
                                            size_t body_remaining =
                                                    self->_msg_body->totalLen - self->_msg_body->currentLen;
                                            // 计算本次可以读取的消息体字节数
                                            size_t body_to_copy = std::min(
                                                body_remaining, bytes_transferred - buffer_pos);

                                            // 从正确的缓冲区位置拷贝数据
                                            memcpy(self->_msg_body->data + self->_msg_body->currentLen,
                                                   self->_buffer + buffer_pos,
                                                   body_to_copy);

                                            self->_msg_body->currentLen += body_to_copy;
                                            buffer_pos += body_to_copy;

                                            // 如果消息体还没读取完整，等待下次读取
                                            if (self->_msg_body->currentLen < self->_msg_body->totalLen) {
                                                self->read();
                                                return;
                                            }

                                            // 消息完整，处理消息
                                            // 处理认证消息
                                            if (self->_msgId == ID_CHAT_LOGIN) {
                                                std::string msg_str(self->_msg_body->data, self->_msg_body->totalLen);
                                                
                                                try {
                                                    json data = json::parse(msg_str);
                                                    if (data.contains("uid") && data.contains("token")) {
                                                        int uid = data["uid"].get<int>();
                                                        std::string token = data["token"].get<std::string>();
                                                        
                                                        // 验证Token
                                                        auto respond = StatusGrpcClient::getInstance()->checkToken(uid, token);
                                                        if (respond.error() == ErrorCodes::SUCCESS) {
                                                            self->_isAuthenticated = true;
                                                            self->_uid = uid;
                                                            
                                                            // 添加到会话管理器
                                                            SessionMgr::getInstance()->addSession(uid, self);
                                                            
                                                            // 发送登录成功响应
                                                            json response;
                                                            response["error"] = ErrorCodes::SUCCESS;
                                                            response["token"] = token;
                                                            std::string resp_str = response.dump();
                                                            self->send(const_cast<char*>(resp_str.c_str()), resp_str.length(), ID_CHAT_LOGIN_RSP);
                                                            
                                                            // 发送好友列表和未读消息通知
                                                            self->sendInitialData();
                                                        } else {
                                                            // 认证失败
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
                                            } else {
                                                // 非认证消息，必须先认证
                                                if (!self->_isAuthenticated) {
                                                    std::cerr << "用户未认证，拒绝处理消息 ID: " << self->_msgId << std::endl;
                                                    json response;
                                                    response["error"] = ErrorCodes::Token_Invalid;
                                                    std::string resp_str = response.dump();
                                                    self->send(const_cast<char*>(resp_str.c_str()), resp_str.length(), self->_msgId + 1);
                                                } else {
                                                    std::cout << "接收到消息 ID:" << self->_msgId << std::endl;
                                                    
                                                    // 创建逻辑节点并分发到消息队列
                                                    std::shared_ptr<LogicNode> logicNode = std::make_shared<LogicNode>(
                                                        self->_msgId, self->_msg_body, self);
                                                        
                                                    LogicSystem::getInstance()->postMsgToQueue(logicNode);
                                                }
                                            }

                                            // 重置状态，准备处理下一条消息
                                            self->_isParsed = false;
                                            self->_msg_head->clear();
                                            self->_msg_body->clear();
                                        }
                                    }
                                    // 当前缓冲区已处理完毕，继续读取新数据
                                    self->read();
                                } catch (std::exception &e) {
                                    std::cerr << "read error:" << e.what() << std::endl;
                                    self->_isParsed = false; // 发生异常时重置状态
                                    self->_msg_head->clear();
                                    self->_msg_body->clear();
                                    self->read();
                                }
                            });
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

        char *msgData=new char[totalLen+4];
        short network_id = htons(id);
        short network_len = htons(totalLen);
        memcpy(msgData,&network_id,2);
        memcpy(msgData+2,&network_len,2);
        memcpy(msgData+4,data,totalLen);
        _send_queue.emplace(std::make_shared<SendNode>(msgData, id, totalLen+4));
        // 等待前序节点发送完成后pop
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
        // 发送好友列表
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
        
        // 发送未处理的好友请求
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
        
        // 获取最近消息
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
