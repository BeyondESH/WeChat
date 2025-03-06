//
// Created by Beyond on 2024/11/10.
//
#include "VerifyGrpcClient.h"
#include "LogicSystem.h"
#include "CSession.h"
#include "MySQLMgr.h"
#include "RedisMgr.h"
#include "Crypto.h"
#include "StatusGrpcClient.h"
using json = nlohmann::json;

bool LogicSystem::handleGet(std::string path, std::shared_ptr<CSession> connection) {
    //未找到path
    if (_getHandlers.find(path) == _getHandlers.end()) {
        return false;
    }
    _getHandlers[path](connection);
    return true;
}

bool LogicSystem::handlePost(std::string path, std::shared_ptr<CSession> connection) {
    //未找到path
    if (_postHandlers.find(path) == _postHandlers.end()) {
        return false;
    }
    _postHandlers[path](connection);
    return true;
}

LogicSystem::~LogicSystem() {
}

void LogicSystem::postMsgToQueue(std::shared_ptr<LogicNode> logicNode) {

}

void LogicSystem::regGet(std::string url, HttpHandler handler) {
    _getHandlers.insert({url, handler});
}

LogicSystem::LogicSystem() {

}

void LogicSystem::regPost(std::string url, HttpHandler handler) {
    _postHandlers.insert({url, handler});
}
