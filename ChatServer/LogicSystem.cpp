//
// Created by Beyond on 2024/11/10.
//
#include "VerifyGrpcClient.h"
#include "LogicSystem.h"
#include "CSession.h"
#include "MySQLMgr.h"
#include "RedisMgr.h"
#include "Crypto.h"
#include "LogicNode.h"
#include "StatusGrpcClient.h"
#include "MsgNode.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

LogicSystem::~LogicSystem()
{
    close();
}

bool LogicSystem::handleMsg(std::shared_ptr<LogicNode> logicNode)
{
    try
    {
        auto id = logicNode.get()->_id;
        auto iter = _msgHandlers.find(id);
        if (iter == _msgHandlers.end())
        {
            std::cerr << "LogicNode handler for ID " << id << " does not exist" << std::endl;
            return false;
        }
        else
        {
            auto msgNode = logicNode.get()->_msgNode;
            auto session = logicNode.get()->_session;
            iter->second(msgNode, session);
            return true;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "handle msg error:" << e.what() << std::endl;
        return false;
    }
}

void LogicSystem::postMsgToQueue(std::shared_ptr<LogicNode> logicNode)
{
    try
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_queue.size() > QUEUE_MAX_SIZE)
        {
            std::cerr << "queue is full, drop msg!" << std::endl;
            return;
        }
        _queue.emplace(logicNode);
        _cv.notify_one();
    }
    catch (const std::error_code &ec)
    {
        std::cerr << "post msg to queue error:" << ec.value() << std::endl;
    }
}

LogicSystem::LogicSystem() : _isStop(false)
{
    try
    {
        auto registerHandlers = [this]()
        {
            // 处理好友列表请求
            _msgHandlers[ID_FRIEND_LIST_REQ] = [](std::shared_ptr<MsgNode> msgNode, std::shared_ptr<CSession> session)
            {
                std::cout << "处理ID_FRIEND_LIST_REQ消息" << std::endl;
                std::string msg_str(msgNode.get()->data, msgNode.get()->totalLen);

                try
                {
                    json src_root = json::parse(msg_str);
                    int uid = src_root["uid"].get<int>();

                    // 获取好友列表
                    auto friends = MySQLMgr::getInstance()->getFriendList(uid);

                    // 构建响应
                    json friendsJson = json::array();
                    for (const auto &f : friends)
                    {
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

                    session->send(const_cast<char *>(resp_str.c_str()), resp_str.length(), ID_FRIEND_LIST_RSP);
                }
                catch (const std::exception &e)
                {
                    std::cerr << "处理好友列表请求时出错：" << e.what() << std::endl;
                    json response;
                    response["error"] = ErrorCodes::ERROR_JSON;
                    std::string resp_str = response.dump();
                    session->send(const_cast<char *>(resp_str.c_str()), resp_str.length(), ID_FRIEND_LIST_RSP);
                }
            };

            _msgHandlers[ID_CHAT_LOGIN] = [](std::shared_ptr<MsgNode> msgNode, std::shared_ptr<CSession> session)
            {
                std::cout << "handle ID_CHAT_LOGIN msg" << std::endl;
                std::string msg_str(msgNode.get()->data, msgNode.get()->totalLen);
                if (msg_str.empty())
                {
                    std::cerr << "接收到空消息" << std::endl;
                    json root;
                    root["error"] = ErrorCodes::ERROR_JSON; // 错误代码
                    std::string json_str = root.dump(4);
                    session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_CHAT_LOGIN_RSP);
                    return;
                }
                std::cout << "handle msg_str:" << msg_str << std::endl;
                json src_root = json::parse(msg_str);
                json root;
                int uid = src_root["uid"].get<int>();
                std::string token = src_root["token"];
                auto respond = StatusGrpcClient::getInstance()->checkToken(uid, token); // 查询并验证token
                std::cout << "check token result is:" << respond.error() << std::endl;
                root["error"] = respond.error();
                std::string json_str = root.dump(4);
                std::cout << "sent json is:" << json_str << std::endl;
                short totalLen = strlen(json_str.c_str());
                session->send(const_cast<char *>(json_str.c_str()), totalLen, ID_CHAT_LOGIN_RSP);
            };

            // 搜索用户处理器
            _msgHandlers[ID_SEARCH_USER_REQ] = [](std::shared_ptr<MsgNode> msgNode, std::shared_ptr<CSession> session)
            {
                std::cout << "处理ID_SEARCH_USER_REQ消息" << std::endl;
                std::string msg_str(msgNode.get()->data, msgNode.get()->totalLen);
                json src_root = json::parse(msg_str);
                json root;

                std::string searchKey = src_root["keyword"].get<std::string>();
                int searchType = src_root["searchType"].get<int>();

                // 调用MySQL查找用户
                std::vector<UserInfo> results;
                if (searchType == 0)
                { // 按用户名搜索
                    results = MySQLMgr::getInstance()->searchUserByName(searchKey);
                }
                else if (searchType == 1)
                { // 按用户ID搜索
                    try
                    {
                        int uid = std::stoi(searchKey);
                        auto user = MySQLMgr::getInstance()->getUserById(uid);
                        if (user.uid > 0)
                        {
                            results.push_back(user);
                        }
                    }
                    catch (...)
                    {
                        // 转换失败处理
                    }
                }

                // 构建响应
                root["error"] = ErrorCodes::SUCCESS;
                json usersArray = json::array();
                for (const auto &user : results)
                {
                    json userJson;
                    userJson["uid"] = user.uid;
                    userJson["name"] = user.name;
                    userJson["avatar"] = user.avatar;
                    usersArray.push_back(userJson);
                }
                root["users"] = usersArray;

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_SEARCH_USER_RSP);
            };

            // 添加好友请求处理器
            _msgHandlers[ID_ADD_FRIEND_REQ] = [](std::shared_ptr<MsgNode> msgNode, std::shared_ptr<CSession> session)
            {
                std::cout << "处理ID_ADD_FRIEND_REQ消息" << std::endl;
                std::string msg_str(msgNode.get()->data, msgNode.get()->totalLen);
                json src_root = json::parse(msg_str);
                json root;

                int fromUid = src_root["fromUid"].get<int>();
                int toUid = src_root["toUid"].get<int>();
                std::string message = src_root["message"].get<std::string>();

                // 保存好友请求到数据库
                bool success = MySQLMgr::getInstance()->addFriendRequest(fromUid, toUid, message);

                if (success)
                {
                    root["error"] = ErrorCodes::SUCCESS;

                    // 获取发送方用户信息
                    UserInfo fromUser = MySQLMgr::getInstance()->getUserById(fromUid);

                    // 检查对方是否在线，如果在线则发送通知
                    auto targetSession = SessionMgr::getInstance()->getSessionByUid(toUid);
                    if (targetSession)
                    {
                        json notifyJson;
                        notifyJson["fromUid"] = fromUid;
                        notifyJson["fromName"] = fromUser.name;
                        notifyJson["fromAvatar"] = fromUser.avatar;
                        notifyJson["message"] = message;
                        notifyJson["requestTime"] = getCurrentTimeString();

                        std::string notifyStr = notifyJson.dump();
                        targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_ADD_FRIEND_REQ);
                    }
                }
                else
                {
                    root["error"] = ErrorCodes::ERR_NETWORK;
                }

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_ADD_FRIEND_RSP);
            };

            // 处理好友请求响应
            _msgHandlers[ID_AUTH_FRIEND_REQ] = [](std::shared_ptr<MsgNode> msgNode, std::shared_ptr<CSession> session)
            {
                std::cout << "处理ID_AUTH_FRIEND_REQ消息" << std::endl;
                std::string msg_str(msgNode.get()->data, msgNode.get()->totalLen);
                json src_root = json::parse(msg_str);
                json root;

                int requestId = src_root["requestId"].get<int>();
                int fromUid = src_root["fromUid"].get<int>();
                int toUid = src_root["toUid"].get<int>();
                bool accepted = src_root["accept"].get<bool>();

                // 更新好友请求状态
                bool success = MySQLMgr::getInstance()->updateFriendRequestStatus(requestId, accepted);

                if (success && accepted)
                {
                    // 如果接受请求，则添加好友关系
                    success = MySQLMgr::getInstance()->addFriendRelation(fromUid, toUid);
                }

                if (success)
                {
                    root["error"] = ErrorCodes::SUCCESS;

                    // 获取双方用户信息
                    UserInfo fromUser = MySQLMgr::getInstance()->getUserById(fromUid);
                    UserInfo toUser = MySQLMgr::getInstance()->getUserById(toUid);

                    // 通知对方请求被处理
                    auto targetSession = SessionMgr::getInstance()->getSessionByUid(fromUid);
                    if (targetSession)
                    {
                        json notifyJson;
                        notifyJson["requestId"] = requestId;
                        notifyJson["fromUid"] = toUid;
                        notifyJson["fromName"] = toUser.name;
                        notifyJson["toUid"] = fromUid;
                        notifyJson["accepted"] = accepted;

                        if (accepted)
                        {
                            notifyJson["message"] = "对方已接受您的好友请求";
                        }
                        else
                        {
                            notifyJson["message"] = "对方已拒绝您的好友请求";
                        }

                        std::string notifyStr = notifyJson.dump();
                        targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_AUTH_FRIEND_REQ);
                    }
                }
                else
                {
                    root["error"] = ErrorCodes::ERR_NETWORK;
                }

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_AUTH_FRIEND_RSP);
            };

            // 处理文本聊天消息
            _msgHandlers[ID_TEXT_CHAT_MSG_REQ] = [](std::shared_ptr<MsgNode> msgNode, std::shared_ptr<CSession> session)
            {
                std::cout << "处理ID_TEXT_CHAT_MSG_REQ消息" << std::endl;
                std::string msg_str(msgNode.get()->data, msgNode.get()->totalLen);
                json src_root = json::parse(msg_str);
                json root;

                int fromUid = src_root["fromUid"].get<int>();
                int toUid = src_root["toUid"].get<int>();
                std::string content = src_root["content"].get<std::string>();
                std::string time = getCurrentTimeString();

                // 保存消息到数据库
                int messageId = MySQLMgr::getInstance()->saveMessage(fromUid, toUid, content, time, 0);

                if (messageId > 0)
                {
                    root["error"] = ErrorCodes::SUCCESS;
                    root["messageId"] = messageId;
                    root["time"] = time;

                    // 获取发送方用户信息
                    UserInfo fromUser = MySQLMgr::getInstance()->getUserById(fromUid);

                    // 检查接收方是否在线，如果在线则立即发送消息
                    auto targetSession = SessionMgr::getInstance()->getSessionByUid(toUid);
                    if (targetSession)
                    {
                        json notifyJson;
                        notifyJson["messageId"] = messageId;
                        notifyJson["fromUid"] = fromUid;
                        notifyJson["fromName"] = fromUser.name;
                        notifyJson["fromAvatar"] = fromUser.avatar;
                        notifyJson["content"] = content;
                        notifyJson["time"] = time;

                        std::string notifyStr = notifyJson.dump();
                        targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_TEXT_CHAT_MSG_REQ);
                    }
                }
                else
                {
                    root["error"] = ErrorCodes::ERR_NETWORK;
                }

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_TEXT_CHAT_MSG_RSP);
            };

            // 处理聊天历史请求
            _msgHandlers[ID_GET_CHAT_HISTORY_REQ] = [](std::shared_ptr<MsgNode> msgNode, std::shared_ptr<CSession> session)
            {
                std::cout << "处理ID_GET_CHAT_HISTORY_REQ消息" << std::endl;
                std::string msg_str(msgNode.get()->data, msgNode.get()->totalLen);

                try
                {
                    json src_root = json::parse(msg_str);
                    json root;

                    int uid = src_root["uid"].get<int>();
                    int contactUid = src_root["contactUid"].get<int>();
                    int count = src_root["count"].get<int>();
                    int offset = src_root["offset"].get<int>();

                    // 获取历史消息
                    auto messages = MySQLMgr::getInstance()->getChatHistory(uid, contactUid, count, offset);

                    // 构建响应
                    root["error"] = ErrorCodes::SUCCESS;
                    root["contact_uid"] = contactUid;
                    json messagesJson = json::array();

                    for (const auto &msg : messages)
                    {
                        json msgJson;
                        msgJson["id"] = msg.at("id");
                        msgJson["from_uid"] = msg.at("from_uid");
                        msgJson["to_uid"] = msg.at("to_uid");
                        msgJson["content"] = msg.at("content");
                        msgJson["send_time"] = msg.at("send_time");
                        msgJson["msg_type"] = msg.at("msg_type");
                        msgJson["status"] = msg.at("status");
                        messagesJson.push_back(msgJson);
                    }

                    root["messages"] = messagesJson;

                    std::string json_str = root.dump();
                    session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_GET_CHAT_HISTORY_RSP);
                }
                catch (const std::exception &e)
                {
                    std::cerr << "处理聊天历史请求时出错：" << e.what() << std::endl;
                    json root;
                    root["error"] = ErrorCodes::ERROR_JSON;
                    std::string json_str = root.dump();
                    session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_GET_CHAT_HISTORY_RSP);
                }
            };
        };

        // 注册消息处理器
        registerHandlers();

        // 创建消息处理线程
        std::thread([this]()
                    {
                    while (!_isStop) {
                        std::shared_ptr<LogicNode> logicNode; {
                            std::unique_lock<std::mutex> lock(_mutex);
                            _cv.wait(lock, [this]() {
                                return _isStop || !_queue.empty();
                            });
                            if (_isStop) {
                                break;
                            }
                            if (!_queue.empty()) {
                                logicNode = std::move(_queue.front());
                                _queue.pop();
                            }
                        }
                        if (logicNode) {
                            handleMsg(logicNode);
                        }
                    }
                    // 处理剩余消息
                    std::lock_guard<std::mutex> lock(_mutex);
                    while (!_queue.empty()) {
                        auto logicNode = std::move(_queue.front());
                        handleMsg(logicNode);
                        _queue.pop();
                    } })
            .detach();
    }
    catch (const std::exception &e)
    {
        std::cerr << "LogicSystem exception:" << e.what() << std::endl;
    }
}

void LogicSystem::close()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _isStop = true;
    _cv.notify_all();
}
