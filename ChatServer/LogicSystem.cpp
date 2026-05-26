#include "VerifyGrpcClient.h"
#include "LogicSystem.h"
#include "CSession.h"
#include "MySQLMgr.h"
#include "RedisMgr.h"
#include "Crypto.h"
#include "LogicNode.h"
#include "StatusGrpcClient.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

LogicSystem::~LogicSystem() {
    close();
}

bool LogicSystem::handleMsg(std::shared_ptr<LogicNode> logicNode) {
    try {
        auto id = logicNode->getId();
        auto iter = _msgHandlers.find(id);
        if (iter == _msgHandlers.end()) {
            std::cerr << "LogicNode handler for ID " << id << " does not exist" << std::endl;
            return false;
        } else {
            auto body = logicNode->getBody();
            auto session = logicNode->getSession();
            if (session) {
                iter->second(body, session);
            }
            return true;
        }
    } catch (const std::exception &e) {
        std::cerr << "handle msg error:" << e.what() << std::endl;
        return false;
    }
}

void LogicSystem::postMsgToQueue(std::shared_ptr<LogicNode> logicNode) {
    try {
        int uid = 0;
        auto session = logicNode->getSession();
        if (session) {
            uid = session->getUid();
        }

        size_t queueIndex = getQueueIndex(uid);
        
        {
            std::lock_guard<std::mutex> lock(_queueMutexes[queueIndex]);
            if (_queues[queueIndex].size() > QUEUE_MAX_SIZE) {
                std::cerr << "queue " << queueIndex << " is full, drop msg!" << std::endl;
                return;
            }
            _queues[queueIndex].emplace(logicNode);
        }
        _queueCvs[queueIndex].notify_one();
    } catch (const std::error_code &ec) {
        std::cerr << "post msg to queue error:" << ec.value() << std::endl;
    }
}

void LogicSystem::workerThread(int queueIndex) {
    while (!_isStop) {
        std::shared_ptr<LogicNode> logicNode;
        {
            std::unique_lock<std::mutex> lock(_queueMutexes[queueIndex]);
            _queueCvs[queueIndex].wait(lock, [this, queueIndex]() {
                return _isStop || !_queues[queueIndex].empty();
            });
            if (_isStop) {
                break;
            }
            if (!_queues[queueIndex].empty()) {
                logicNode = std::move(_queues[queueIndex].front());
                _queues[queueIndex].pop();
            }
        }
        if (logicNode) {
            handleMsg(logicNode);
        }
    }

    std::lock_guard<std::mutex> lock(_queueMutexes[queueIndex]);
    while (!_queues[queueIndex].empty()) {
        auto logicNode = std::move(_queues[queueIndex].front());
        handleMsg(logicNode);
        _queues[queueIndex].pop();
    }
}

size_t LogicSystem::getQueueIndex(int uid) const {
    if (uid <= 0) {
        return 0;
    }
    return static_cast<size_t>(uid) % _workerCount;
}

LogicSystem::LogicSystem() : _isStop(false), _workerCount(std::thread::hardware_concurrency()) {
    if (_workerCount == 0) {
        _workerCount = 4;
    }

    try {
        _queues.resize(_workerCount);
        _queueMutexes.resize(_workerCount);
        _queueCvs.resize(_workerCount);

        auto registerHandlers = [this]() {
            _msgHandlers[ID_FRIEND_LIST_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_FRIEND_LIST_REQ消息" << std::endl;

                try {
                    json src_root = json::parse(msg_str);
                    int uid = src_root["uid"].get<int>();

                    auto friends = MySQLMgr::getInstance()->getFriendList(uid);

                    json friendsJson = json::array();
                    for (const auto &f : friends) {
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
                } catch (const std::exception &e) {
                    std::cerr << "处理好友列表请求时出错：" << e.what() << std::endl;
                    json response;
                    response["error"] = ErrorCodes::ERROR_JSON;
                    std::string resp_str = response.dump();
                    session->send(const_cast<char *>(resp_str.c_str()), resp_str.length(), ID_FRIEND_LIST_RSP);
                }
            };

            _msgHandlers[ID_SEARCH_USER_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_SEARCH_USER_REQ消息" << std::endl;
                json src_root = json::parse(msg_str);
                json root;

                std::string searchKey = src_root["keyword"].get<std::string>();
                int searchType = src_root["searchType"].get<int>();

                std::vector<UserInfo> results;
                if (searchType == 0) {
                    results = MySQLMgr::getInstance()->searchUserByName(searchKey);
                } else if (searchType == 1) {
                    try {
                        int uid = std::stoi(searchKey);
                        auto user = MySQLMgr::getInstance()->getUserById(uid);
                        if (user.uid > 0) {
                            results.push_back(user);
                        }
                    } catch (...) {
                    }
                }

                root["error"] = ErrorCodes::SUCCESS;
                json usersArray = json::array();
                for (const auto &user : results) {
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

            _msgHandlers[ID_ADD_FRIEND_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_ADD_FRIEND_REQ消息" << std::endl;
                json src_root = json::parse(msg_str);
                json root;

                int fromUid = src_root["fromUid"].get<int>();
                int toUid = src_root["toUid"].get<int>();
                std::string message = src_root["message"].get<std::string>();

                bool success = MySQLMgr::getInstance()->addFriendRequest(fromUid, toUid, message);

                if (success) {
                    root["error"] = ErrorCodes::SUCCESS;

                    UserInfo fromUser = MySQLMgr::getInstance()->getUserById(fromUid);

                    auto targetSession = SessionMgr::getInstance()->getSessionByUid(toUid);
                    if (targetSession) {
                        json notifyJson;
                        notifyJson["fromUid"] = fromUid;
                        notifyJson["fromName"] = fromUser.name;
                        notifyJson["fromAvatar"] = fromUser.avatar;
                        notifyJson["message"] = message;
                        notifyJson["requestTime"] = getCurrentTimeString();

                        std::string notifyStr = notifyJson.dump();
                        targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_ADD_FRIEND_REQ);
                    } else {
                        json redisMsg;
                        redisMsg["type"] = "friend_request";
                        redisMsg["fromUid"] = fromUid;
                        redisMsg["fromName"] = fromUser.name;
                        redisMsg["fromAvatar"] = fromUser.avatar;
                        redisMsg["toUid"] = toUid;
                        redisMsg["message"] = message;
                        redisMsg["requestTime"] = getCurrentTimeString();
                        std::string channel = "user:" + std::to_string(toUid);
                        RedisMgr::getInstance()->publish(channel, redisMsg.dump());
                    }
                } else {
                    root["error"] = ErrorCodes::ERR_NETWORK;
                }

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_ADD_FRIEND_RSP);
            };

            _msgHandlers[ID_AUTH_FRIEND_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_AUTH_FRIEND_REQ消息" << std::endl;
                json src_root = json::parse(msg_str);
                json root;

                int requestId = src_root["requestId"].get<int>();
                int fromUid = src_root["fromUid"].get<int>();
                int toUid = src_root["toUid"].get<int>();
                bool accepted = src_root["accept"].get<bool>();

                bool success = MySQLMgr::getInstance()->updateFriendRequestStatus(requestId, accepted);

                if (success && accepted) {
                    success = MySQLMgr::getInstance()->addFriendRelation(fromUid, toUid);
                }

                if (success) {
                    root["error"] = ErrorCodes::SUCCESS;

                    UserInfo fromUser = MySQLMgr::getInstance()->getUserById(fromUid);
                    UserInfo toUser = MySQLMgr::getInstance()->getUserById(toUid);

                    auto targetSession = SessionMgr::getInstance()->getSessionByUid(fromUid);
                    if (targetSession) {
                        json notifyJson;
                        notifyJson["requestId"] = requestId;
                        notifyJson["fromUid"] = toUid;
                        notifyJson["fromName"] = toUser.name;
                        notifyJson["toUid"] = fromUid;
                        notifyJson["accepted"] = accepted;

                        if (accepted) {
                            notifyJson["message"] = "对方已接受您的好友请求";
                        } else {
                            notifyJson["message"] = "对方已拒绝您的好友请求";
                        }

                        std::string notifyStr = notifyJson.dump();
                        targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_AUTH_FRIEND_RSP);
                    } else {
                        json redisMsg;
                        redisMsg["type"] = "auth_friend_rsp";
                        redisMsg["requestId"] = requestId;
                        redisMsg["fromUid"] = fromUid;
                        redisMsg["toUid"] = toUid;
                        redisMsg["toName"] = toUser.name;
                        redisMsg["accepted"] = accepted;
                        if (accepted) {
                            redisMsg["message"] = "对方已接受您的好友请求";
                        } else {
                            redisMsg["message"] = "对方已拒绝您的好友请求";
                        }
                        std::string channel = "user:" + std::to_string(fromUid);
                        RedisMgr::getInstance()->publish(channel, redisMsg.dump());
                    }
                } else {
                    root["error"] = ErrorCodes::ERR_NETWORK;
                }

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_AUTH_FRIEND_RSP);
            };

            _msgHandlers[ID_TEXT_CHAT_MSG_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_TEXT_CHAT_MSG_REQ消息" << std::endl;
                json src_root = json::parse(msg_str);
                json root;

                int fromUid = src_root["fromUid"].get<int>();
                int toUid = src_root["toUid"].get<int>();
                std::string content = src_root["content"].get<std::string>();
                std::string time = getCurrentTimeString();

                int messageId = MySQLMgr::getInstance()->saveMessage(fromUid, toUid, content, time, 0);

                if (messageId > 0) {
                    root["error"] = ErrorCodes::SUCCESS;
                    root["messageId"] = messageId;
                    root["time"] = time;

                    UserInfo fromUser = MySQLMgr::getInstance()->getUserById(fromUid);

                    auto targetSession = SessionMgr::getInstance()->getSessionByUid(toUid);
                    if (targetSession) {
                        json notifyJson;
                        notifyJson["messageId"] = messageId;
                        notifyJson["fromUid"] = fromUid;
                        notifyJson["fromName"] = fromUser.name;
                        notifyJson["fromAvatar"] = fromUser.avatar;
                        notifyJson["content"] = content;
                        notifyJson["time"] = time;

                        std::string notifyStr = notifyJson.dump();
                        targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_TEXT_CHAT_MSG_REQ);
                    } else {
                        json redisMsg;
                        redisMsg["type"] = "chat_msg";
                        redisMsg["messageId"] = messageId;
                        redisMsg["fromUid"] = fromUid;
                        redisMsg["fromName"] = fromUser.name;
                        redisMsg["fromAvatar"] = fromUser.avatar;
                        redisMsg["toUid"] = toUid;
                        redisMsg["content"] = content;
                        redisMsg["time"] = time;
                        std::string channel = "user:" + std::to_string(toUid);
                        RedisMgr::getInstance()->publish(channel, redisMsg.dump());
                    }
                } else {
                    root["error"] = ErrorCodes::ERR_NETWORK;
                }

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_TEXT_CHAT_MSG_RSP);
            };

            _msgHandlers[ID_IMAGE_CHAT_MSG_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_IMAGE_CHAT_MSG_REQ消息" << std::endl;
                json src_root = json::parse(msg_str);
                json root;

                int fromUid = src_root["fromUid"].get<int>();
                int toUid = src_root["toUid"].get<int>();
                std::string base64Content = src_root["content"].get<std::string>();
                std::string imageType = src_root["imageType"].get<std::string>();
                int imageWidth = src_root["width"].get<int>();
                int imageHeight = src_root["height"].get<int>();
                int fileSize = src_root["fileSize"].get<int>();
                std::string time = getCurrentTimeString();

                int messageId = MySQLMgr::getInstance()->saveMessage(fromUid, toUid, base64Content, time, 1);

                if (messageId > 0) {
                    root["error"] = ErrorCodes::SUCCESS;
                    root["messageId"] = messageId;
                    root["time"] = time;

                    UserInfo fromUser = MySQLMgr::getInstance()->getUserById(fromUid);

                    auto targetSession = SessionMgr::getInstance()->getSessionByUid(toUid);
                    if (targetSession) {
                        json notifyJson;
                        notifyJson["messageId"] = messageId;
                        notifyJson["fromUid"] = fromUid;
                        notifyJson["fromName"] = fromUser.name;
                        notifyJson["fromAvatar"] = fromUser.avatar;
                        notifyJson["content"] = base64Content;
                        notifyJson["imageType"] = imageType;
                        notifyJson["width"] = imageWidth;
                        notifyJson["height"] = imageHeight;
                        notifyJson["fileSize"] = fileSize;
                        notifyJson["time"] = time;

                        std::string notifyStr = notifyJson.dump();
                        targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_IMAGE_CHAT_MSG_REQ);
                    } else {
                        json redisMsg;
                        redisMsg["type"] = "image_msg";
                        redisMsg["messageId"] = messageId;
                        redisMsg["fromUid"] = fromUid;
                        redisMsg["fromName"] = fromUser.name;
                        redisMsg["fromAvatar"] = fromUser.avatar;
                        redisMsg["toUid"] = toUid;
                        redisMsg["content"] = base64Content;
                        redisMsg["imageType"] = imageType;
                        redisMsg["width"] = imageWidth;
                        redisMsg["height"] = imageHeight;
                        redisMsg["fileSize"] = fileSize;
                        redisMsg["time"] = time;
                        std::string channel = "user:" + std::to_string(toUid);
                        RedisMgr::getInstance()->publish(channel, redisMsg.dump());
                    }
                } else {
                    root["error"] = ErrorCodes::ERR_NETWORK;
                }

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_IMAGE_CHAT_MSG_RSP);
            };

            _msgHandlers[ID_GET_CHAT_HISTORY_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_GET_CHAT_HISTORY_REQ消息" << std::endl;

                try {
                    json src_root = json::parse(msg_str);
                    json root;

                    int uid = src_root["uid"].get<int>();
                    int contactUid = src_root["contactUid"].get<int>();
                    int count = src_root["count"].get<int>();
                    int offset = src_root["offset"].get<int>();

                    auto messages = MySQLMgr::getInstance()->getChatHistory(uid, contactUid, count, offset);

                    root["error"] = ErrorCodes::SUCCESS;
                    root["contact_uid"] = contactUid;
                    json messagesJson = json::array();

                    for (const auto &msg : messages) {
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
                } catch (const std::exception &e) {
                    std::cerr << "处理聊天历史请求时出错：" << e.what() << std::endl;
                    json root;
                    root["error"] = ErrorCodes::ERROR_JSON;
                    std::string json_str = root.dump();
                    session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_GET_CHAT_HISTORY_RSP);
                }
            };

            _msgHandlers[ID_CREATE_GROUP_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_CREATE_GROUP_REQ消息" << std::endl;
                json src_root = json::parse(msg_str);
                json root;

                int ownerUid = src_root["ownerUid"].get<int>();
                std::string name = src_root["name"].get<std::string>();
                std::string avatar = src_root.value("avatar", std::string(""));

                int groupId = MySQLMgr::getInstance()->createGroup(ownerUid, name, avatar);

                if (groupId > 0) {
                    root["error"] = ErrorCodes::SUCCESS;
                    root["groupId"] = groupId;
                    root["name"] = name;
                } else {
                    root["error"] = ErrorCodes::ERR_NETWORK;
                }

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_CREATE_GROUP_RSP);
            };

            _msgHandlers[ID_ADD_GROUP_MEMBER_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_ADD_GROUP_MEMBER_REQ消息" << std::endl;
                json src_root = json::parse(msg_str);
                json root;

                int groupId = src_root["groupId"].get<int>();
                std::vector<int> uids = src_root["uids"].get<std::vector<int>>();

                bool success = true;
                for (int uid : uids) {
                    if (!MySQLMgr::getInstance()->addGroupMember(groupId, uid)) {
                        success = false;
                        break;
                    }
                }

                root["error"] = success ? ErrorCodes::SUCCESS : ErrorCodes::ERR_NETWORK;

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_ADD_GROUP_MEMBER_RSP);
            };

            _msgHandlers[ID_REMOVE_GROUP_MEMBER_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_REMOVE_GROUP_MEMBER_REQ消息" << std::endl;
                json src_root = json::parse(msg_str);
                json root;

                int groupId = src_root["groupId"].get<int>();
                int uid = src_root["uid"].get<int>();

                bool success = MySQLMgr::getInstance()->removeGroupMember(groupId, uid);

                root["error"] = success ? ErrorCodes::SUCCESS : ErrorCodes::ERR_NETWORK;

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_REMOVE_GROUP_MEMBER_RSP);
            };

            _msgHandlers[ID_GROUP_CHAT_MSG_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_GROUP_CHAT_MSG_REQ消息" << std::endl;
                json src_root = json::parse(msg_str);
                json root;

                int fromUid = src_root["fromUid"].get<int>();
                int groupId = src_root["groupId"].get<int>();
                std::string content = src_root["content"].get<std::string>();
                std::string time = getCurrentTimeString();

                int messageId = MySQLMgr::getInstance()->saveGroupMessage(fromUid, groupId, content, time, 0);

                if (messageId > 0) {
                    root["error"] = ErrorCodes::SUCCESS;
                    root["messageId"] = messageId;
                    root["time"] = time;

                    UserInfo fromUser = MySQLMgr::getInstance()->getUserById(fromUid);
                    GroupInfo group = MySQLMgr::getInstance()->getGroupInfo(groupId);

                    auto members = MySQLMgr::getInstance()->getGroupMembers(groupId);

                    json notifyJson;
                    notifyJson["messageId"] = messageId;
                    notifyJson["fromUid"] = fromUid;
                    notifyJson["fromName"] = fromUser.name;
                    notifyJson["fromAvatar"] = fromUser.avatar;
                    notifyJson["groupId"] = groupId;
                    notifyJson["groupName"] = group.name;
                    notifyJson["content"] = content;
                    notifyJson["time"] = time;

                    std::string notifyStr = notifyJson.dump();

                    for (const auto &member : members) {
                        if (member.uid == fromUid) continue;

                        auto targetSession = SessionMgr::getInstance()->getSessionByUid(member.uid);
                        if (targetSession) {
                            targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_GROUP_CHAT_MSG_REQ);
                        } else {
                            json redisMsg;
                            redisMsg["type"] = "group_msg";
                            redisMsg["messageId"] = messageId;
                            redisMsg["fromUid"] = fromUid;
                            redisMsg["fromName"] = fromUser.name;
                            redisMsg["fromAvatar"] = fromUser.avatar;
                            redisMsg["groupId"] = groupId;
                            redisMsg["groupName"] = group.name;
                            redisMsg["content"] = content;
                            redisMsg["time"] = time;
                            std::string channel = "user:" + std::to_string(member.uid);
                            RedisMgr::getInstance()->publish(channel, redisMsg.dump());
                        }
                    }
                } else {
                    root["error"] = ErrorCodes::ERR_NETWORK;
                }

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_GROUP_CHAT_MSG_RSP);
            };

            _msgHandlers[ID_GROUP_IMAGE_CHAT_MSG_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_GROUP_IMAGE_CHAT_MSG_REQ消息" << std::endl;
                json src_root = json::parse(msg_str);
                json root;

                int fromUid = src_root["fromUid"].get<int>();
                int groupId = src_root["groupId"].get<int>();
                std::string base64Content = src_root["content"].get<std::string>();
                std::string imageType = src_root["imageType"].get<std::string>();
                int imageWidth = src_root["width"].get<int>();
                int imageHeight = src_root["height"].get<int>();
                int fileSize = src_root["fileSize"].get<int>();
                std::string time = getCurrentTimeString();

                int messageId = MySQLMgr::getInstance()->saveGroupMessage(fromUid, groupId, base64Content, time, 1);

                if (messageId > 0) {
                    root["error"] = ErrorCodes::SUCCESS;
                    root["messageId"] = messageId;
                    root["time"] = time;

                    UserInfo fromUser = MySQLMgr::getInstance()->getUserById(fromUid);
                    GroupInfo group = MySQLMgr::getInstance()->getGroupInfo(groupId);

                    auto members = MySQLMgr::getInstance()->getGroupMembers(groupId);

                    json notifyJson;
                    notifyJson["messageId"] = messageId;
                    notifyJson["fromUid"] = fromUid;
                    notifyJson["fromName"] = fromUser.name;
                    notifyJson["fromAvatar"] = fromUser.avatar;
                    notifyJson["groupId"] = groupId;
                    notifyJson["groupName"] = group.name;
                    notifyJson["content"] = base64Content;
                    notifyJson["imageType"] = imageType;
                    notifyJson["width"] = imageWidth;
                    notifyJson["height"] = imageHeight;
                    notifyJson["fileSize"] = fileSize;
                    notifyJson["time"] = time;

                    std::string notifyStr = notifyJson.dump();

                    for (const auto &member : members) {
                        if (member.uid == fromUid) continue;

                        auto targetSession = SessionMgr::getInstance()->getSessionByUid(member.uid);
                        if (targetSession) {
                            targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_GROUP_IMAGE_CHAT_MSG_REQ);
                        } else {
                            json redisMsg;
                            redisMsg["type"] = "group_image_msg";
                            redisMsg["messageId"] = messageId;
                            redisMsg["fromUid"] = fromUid;
                            redisMsg["fromName"] = fromUser.name;
                            redisMsg["fromAvatar"] = fromUser.avatar;
                            redisMsg["groupId"] = groupId;
                            redisMsg["groupName"] = group.name;
                            redisMsg["content"] = base64Content;
                            redisMsg["imageType"] = imageType;
                            redisMsg["width"] = imageWidth;
                            redisMsg["height"] = imageHeight;
                            redisMsg["fileSize"] = fileSize;
                            redisMsg["time"] = time;
                            std::string channel = "user:" + std::to_string(member.uid);
                            RedisMgr::getInstance()->publish(channel, redisMsg.dump());
                        }
                    }
                } else {
                    root["error"] = ErrorCodes::ERR_NETWORK;
                }

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_GROUP_IMAGE_CHAT_MSG_RSP);
            };

            _msgHandlers[ID_GET_GROUP_LIST_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_GET_GROUP_LIST_REQ消息" << std::endl;
                json src_root = json::parse(msg_str);
                json root;

                int uid = src_root["uid"].get<int>();

                auto groups = MySQLMgr::getInstance()->getUserGroups(uid);

                root["error"] = ErrorCodes::SUCCESS;
                json groupsJson = json::array();

                for (const auto &group : groups) {
                    json groupJson;
                    groupJson["groupId"] = group.groupId;
                    groupJson["name"] = group.name;
                    groupJson["ownerUid"] = group.ownerUid;
                    groupJson["avatar"] = group.avatar;
                    groupJson["createdAt"] = group.createdAt;
                    groupsJson.push_back(groupJson);
                }

                root["groups"] = groupsJson;

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_GET_GROUP_LIST_RSP);
            };

            _msgHandlers[ID_GET_GROUP_MEMBERS_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_GET_GROUP_MEMBERS_REQ消息" << std::endl;
                json src_root = json::parse(msg_str);
                json root;

                int groupId = src_root["groupId"].get<int>();

                auto members = MySQLMgr::getInstance()->getGroupMembers(groupId);

                root["error"] = ErrorCodes::SUCCESS;
                root["groupId"] = groupId;
                json membersJson = json::array();

                for (const auto &member : members) {
                    json memberJson;
                    memberJson["uid"] = member.uid;
                    memberJson["name"] = member.name;
                    memberJson["avatar"] = member.avatar;
                    memberJson["status"] = member.status;
                    memberJson["isOnline"] = SessionMgr::getInstance()->isUserOnline(member.uid);
                    membersJson.push_back(memberJson);
                }

                root["members"] = membersJson;

                std::string json_str = root.dump();
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_GET_GROUP_MEMBERS_RSP);
            };

            _msgHandlers[ID_GET_GROUP_CHAT_HISTORY_REQ] = [](const std::string &msg_str, std::shared_ptr<CSession> session) {
                std::cout << "处理ID_GET_GROUP_CHAT_HISTORY_REQ消息" << std::endl;
                json src_root = json::parse(msg_str);
                json root;

                int groupId = src_root["groupId"].get<int>();
                int count = src_root["count"].get<int>();
                int offset = src_root["offset"].get<int>();

                auto messages = MySQLMgr::getInstance()->getGroupChatHistory(groupId, count, offset);

                root["error"] = ErrorCodes::SUCCESS;
                root["groupId"] = groupId;
                json messagesJson = json::array();

                for (const auto &msg : messages) {
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
                session->send(const_cast<char *>(json_str.c_str()), json_str.length(), ID_GET_GROUP_CHAT_HISTORY_RSP);
            };
        };

        registerHandlers();

        for (size_t i = 0; i < _workerCount; ++i) {
            _threads.emplace_back(&LogicSystem::workerThread, this, i);
        }

        RedisMgr::getInstance()->startSubscriber([](const std::string& channel, const std::string& message) {
            try {
                json msg = json::parse(message);
                std::string type = msg.value("type", "");

                if (type == "chat_msg") {
                    int toUid = msg["toUid"].get<int>();
                    auto targetSession = SessionMgr::getInstance()->getSessionByUid(toUid);
                    if (targetSession) {
                        json notifyJson;
                        notifyJson["messageId"] = msg["messageId"];
                        notifyJson["fromUid"] = msg["fromUid"];
                        notifyJson["fromName"] = msg["fromName"];
                        notifyJson["fromAvatar"] = msg["fromAvatar"];
                        notifyJson["content"] = msg["content"];
                        notifyJson["time"] = msg["time"];

                        std::string notifyStr = notifyJson.dump();
                        targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_TEXT_CHAT_MSG_REQ);
                    }
                } else if (type == "group_msg") {
                    int groupId = msg["groupId"].get<int>();
                    auto members = MySQLMgr::getInstance()->getGroupMembers(groupId);

                    json notifyJson;
                    notifyJson["messageId"] = msg["messageId"];
                    notifyJson["fromUid"] = msg["fromUid"];
                    notifyJson["fromName"] = msg["fromName"];
                    notifyJson["fromAvatar"] = msg["fromAvatar"];
                    notifyJson["groupId"] = groupId;
                    notifyJson["groupName"] = msg["groupName"];
                    notifyJson["content"] = msg["content"];
                    notifyJson["time"] = msg["time"];

                    std::string notifyStr = notifyJson.dump();
                    int fromUid = msg["fromUid"].get<int>();

                    for (const auto &member : members) {
                        if (member.uid == fromUid) continue;
                        auto targetSession = SessionMgr::getInstance()->getSessionByUid(member.uid);
                        if (targetSession) {
                            targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_GROUP_CHAT_MSG_REQ);
                        }
                    }
                } else if (type == "image_msg") {
                    int toUid = msg["toUid"].get<int>();
                    auto targetSession = SessionMgr::getInstance()->getSessionByUid(toUid);
                    if (targetSession) {
                        json notifyJson;
                        notifyJson["messageId"] = msg["messageId"];
                        notifyJson["fromUid"] = msg["fromUid"];
                        notifyJson["fromName"] = msg["fromName"];
                        notifyJson["fromAvatar"] = msg["fromAvatar"];
                        notifyJson["content"] = msg["content"];
                        notifyJson["imageType"] = msg["imageType"];
                        notifyJson["width"] = msg["width"];
                        notifyJson["height"] = msg["height"];
                        notifyJson["fileSize"] = msg["fileSize"];
                        notifyJson["time"] = msg["time"];

                        std::string notifyStr = notifyJson.dump();
                        targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_IMAGE_CHAT_MSG_REQ);
                    }
                } else if (type == "group_image_msg") {
                    int groupId = msg["groupId"].get<int>();
                    auto members = MySQLMgr::getInstance()->getGroupMembers(groupId);

                    json notifyJson;
                    notifyJson["messageId"] = msg["messageId"];
                    notifyJson["fromUid"] = msg["fromUid"];
                    notifyJson["fromName"] = msg["fromName"];
                    notifyJson["fromAvatar"] = msg["fromAvatar"];
                    notifyJson["groupId"] = groupId;
                    notifyJson["groupName"] = msg["groupName"];
                    notifyJson["content"] = msg["content"];
                    notifyJson["imageType"] = msg["imageType"];
                    notifyJson["width"] = msg["width"];
                    notifyJson["height"] = msg["height"];
                    notifyJson["fileSize"] = msg["fileSize"];
                    notifyJson["time"] = msg["time"];

                    std::string notifyStr = notifyJson.dump();
                    int fromUid = msg["fromUid"].get<int>();

                    for (const auto &member : members) {
                        if (member.uid == fromUid) continue;
                        auto targetSession = SessionMgr::getInstance()->getSessionByUid(member.uid);
                        if (targetSession) {
                            targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_GROUP_IMAGE_CHAT_MSG_REQ);
                        }
                    }
                } else if (type == "friend_request") {
                    int toUid = msg["toUid"].get<int>();
                    auto targetSession = SessionMgr::getInstance()->getSessionByUid(toUid);
                    if (targetSession) {
                        json notifyJson;
                        notifyJson["fromUid"] = msg["fromUid"];
                        notifyJson["fromName"] = msg["fromName"];
                        notifyJson["fromAvatar"] = msg["fromAvatar"];
                        notifyJson["message"] = msg["message"];
                        notifyJson["requestTime"] = msg["requestTime"];

                        std::string notifyStr = notifyJson.dump();
                        targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_ADD_FRIEND_REQ);
                    }
                } else if (type == "auth_friend_rsp") {
                    int fromUid = msg["fromUid"].get<int>();
                    auto targetSession = SessionMgr::getInstance()->getSessionByUid(fromUid);
                    if (targetSession) {
                        json notifyJson;
                        notifyJson["requestId"] = msg["requestId"];
                        notifyJson["fromUid"] = msg["toUid"];
                        notifyJson["fromName"] = msg["toName"];
                        notifyJson["toUid"] = msg["fromUid"];
                        notifyJson["accepted"] = msg["accepted"];
                        notifyJson["message"] = msg["message"];

                        std::string notifyStr = notifyJson.dump();
                        targetSession->send(const_cast<char *>(notifyStr.c_str()), notifyStr.length(), ID_NOTIFY_AUTH_FRIEND_RSP);
                    }
                }
            } catch (const std::exception &e) {
                std::cerr << "Redis subscriber message error: " << e.what() << std::endl;
            }
        });
    } catch (const std::exception &e) {
        std::cerr << "LogicSystem exception:" << e.what() << std::endl;
    }
}

void LogicSystem::close() {
    _isStop = true;
    for (size_t i = 0; i < _workerCount; ++i) {
        _queueCvs[i].notify_all();
    }
    for (auto &thread : _threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}