#include "StatusServiceImpl.h"
#include "ConfigMgr.hpp"
#include "RedisMgr.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <climits>

std::string generateUUID() {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::string uuid_string = boost::uuids::to_string(uuid);
    return uuid_string;
}

StatusServiceImpl::StatusServiceImpl() {
    auto &configMgr = ConfigMgr::getInstance();
    ChatServer chatServer;
    std::string numberServer = configMgr["ChatServer"]["number"];
    int numberServer_int = stoi(numberServer);
    std::cout << "Number server listening on: " << numberServer_int << std::endl;
    for (int i = 1; i <= numberServer_int; ++i) {
        std::string serverName = "ChatServer" + std::to_string(i);
        chatServer.host = configMgr[serverName]["host"];
        chatServer.port = configMgr[serverName]["port"];
        chatServer.name = configMgr[serverName]["name"];
        chatServer.connectCount = 0;
        _chatServers.emplace(chatServer.name, chatServer);
    }
    if (!_chatServers.empty()) {
        _freeServer = _chatServers.begin()->second;
    }
}

grpc::Status StatusServiceImpl::GetChatServer(grpc::ServerContext *context, const message::GetChatServerReq *request,
                                              message::GetChatServerRsp *response) {
    std::cout << "Run GetChatServer..." << std::endl;
    if (!request) {
        response->set_error(ErrorCodes::RPC_FAILED);
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Request is null");
    }

    auto chatServer = choseChatServer();
    response->set_host(chatServer.host);
    response->set_port(chatServer.port);
    response->set_error(ErrorCodes::SUCCESS);
    std::cout << "GetChatServer success, host:" << chatServer.host << " port:" << chatServer.port << std::endl;

    std::string token = generateUUID();
    response->set_token(token);
    std::cout << "Token is:" << token << std::endl;

    insertToken(request->uid(), token);

    return grpc::Status::OK;
}

grpc::Status StatusServiceImpl::CheckToken(grpc::ServerContext *context, const message::CheckTokenReq *request,
                                          message::CheckTokenRsp *response) {
    int uid = request->uid();
    std::string token = request->token();
    bool result = checkToken(uid, token);
    std::cout << "check result:" << result << std::endl;

    if (result) {
        response->set_error(ErrorCodes::SUCCESS);
        removeToken(uid);
    } else {
        response->set_error(ErrorCodes::Token_Invalid);
    }
    response->set_uid(uid);
    response->set_token(token);
    return grpc::Status::OK;
}

ChatServer StatusServiceImpl::choseChatServer() {
    std::lock_guard<std::mutex> lock(_serverMutex);

    ChatServer selectedServer = _freeServer;
    int minConnections = INT_MAX;

    for (auto &entry : _chatServers) {
        std::string key = "chatserver:" + entry.second.name + ":connections";
        std::string countStr;
        int currentCount = 0;
        
        if (RedisMgr::getInstance()->get(key, countStr)) {
            currentCount = std::stoi(countStr);
        }
        
        if (currentCount < minConnections) {
            minConnections = currentCount;
            selectedServer = entry.second;
        }
    }

    std::string key = "chatserver:" + selectedServer.name + ":connections";
    RedisMgr::getInstance()->hIncrBy(key, "count", 1);

    _freeServer = selectedServer;
    return selectedServer;
}

void StatusServiceImpl::insertToken(int uid, const std::string& token) {
    std::string key = TOKEN_PREFIX + std::to_string(uid);
    RedisMgr::getInstance()->set(key, token, TOKEN_TTL);
}

bool StatusServiceImpl::checkToken(int uid, const std::string& token) {
    std::string key = TOKEN_PREFIX + std::to_string(uid);
    std::string storedToken;
    bool exists = RedisMgr::getInstance()->get(key, storedToken);
    
    if (!exists) {
        return false;
    }
    
    return (storedToken == token);
}

void StatusServiceImpl::removeToken(int uid) {
    std::string key = TOKEN_PREFIX + std::to_string(uid);
    RedisMgr::getInstance()->del(key);
}