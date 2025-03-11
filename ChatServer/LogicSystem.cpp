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

LogicSystem::~LogicSystem() {
    close();
}

bool LogicSystem::handleMsg(std::shared_ptr<LogicNode> logicNode) {
    try {
        auto id = logicNode.get()->_id;
        auto iter = _msgHandlers.find(id);
        if (iter == _msgHandlers.end()) {
            std::cerr << "LogicNode does not exist" << std::endl;
            return false;
        } else {
            auto msgNode = logicNode.get()->_msgNode;
            auto session = logicNode.get()->_session;
            iter->second(msgNode, session);
            return true;
        }
    } catch (const std::exception &e) {
        std::cerr << "handle msg error:" << e.what() << std::endl;
        return false;
    }
}

void LogicSystem::postMsgToQueue(std::shared_ptr<LogicNode> logicNode) {
    try {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_queue.size() > QUEUE_MAX_SIZE) {
            std::cerr << "queue is full, drop msg!" << std::endl;
            return;
        }
        _queue.emplace(logicNode);
        _cv.notify_one();
    } catch (const std::error_code &ec) {
        std::cerr << "post msg to queue error:" << ec.value() << std::endl;
    }
}

LogicSystem::LogicSystem() : _isStop(false) {
    try {
        auto registerHandlers = [this]() {
            _msgHandlers[ID_CHAT_LOGIN] = [](std::shared_ptr<MsgNode> msgNode, std::shared_ptr<CSession> session) {
                std::cout<<"handle ID_CHAT_LOGIN msg" << std::endl;
                std::string msg_str(msgNode.get()->data, msgNode.get()->totalLen);
                if (msg_str.empty()) {
                    std::cerr << "接收到空消息" << std::endl;
                    json root;
                    root["error"] = ErrorCodes::ERROR_JSON;  // 错误代码
                    std::string json_str = root.dump(4);
                    session->send(json_str.data(), json_str.length(), ID_CHAT_LOGIN_RSP);
                    return;
                }
                std::cout << "handle msg_str:" << msg_str << std::endl;
                json src_root = json::parse(msg_str);
                json root;
                int uid = src_root["uid"].get<int>();
                std::string token = src_root["token"];
                auto respond = StatusGrpcClient::getInstance()->checkToken(uid, token); //查询并验证token
                std::cout<<"check token result is:"<<respond.error()<<std::endl;
                root["error"] = respond.error();
                std::string json_str = root.dump(4);
                std::cout << "sent json is:" << json_str << std::endl;
                short totalLen = strlen(json_str.data());
                session->send(json_str.data(), totalLen, ID_CHAT_LOGIN_RSP);
            };
        };

        // 注册消息处理器
        registerHandlers();

        // 创建消息处理线程
        std::thread([this]() {
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
                    }
                }).detach();
    } catch (const std::exception &e) {
        std::cerr << "LogicSystem exception:" << e.what() << std::endl;
    }
}

void LogicSystem::close() {
    std::lock_guard<std::mutex> lock(_mutex);
    _isStop = true;
    _cv.notify_all();
}
