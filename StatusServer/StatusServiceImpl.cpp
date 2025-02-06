//
// Created by Beyond on 2025/2/2.
//

#include "StatusServiceImpl.h"
#include "ConfigMgr.hpp"
#include <boost/uuid.hpp>
std::string generateUUID() {
    boost::uuids::uuid uuid=boost::uuids::random_generator()();
    std::string uuid_string=boost::uuids::to_string(uuid);
    return uuid_string;
}

StatusServiceImpl::StatusServiceImpl(){
    auto &configMgr=ConfigMgr::getInstance();
    ChatServer chatServer;
    std::string numberServer=configMgr["ChatServer"]["number"];
    int numberServer_int=stoi(numberServer);
    for (int i = 0; i <numberServer_int; ++i) {
        std::string serverName="ChatServer"+std::to_string(i);
        chatServer.host=configMgr[serverName]["host"];
        chatServer.port=configMgr[serverName]["port"];
        chatServer.name=configMgr[serverName]["name"];
        _chatServers.emplace(chatServer.name,chatServer);
    }
    _freeServer=_chatServers.begin()->second;
}

grpc::Status StatusServiceImpl::GetChatServer(grpc::ServerContext *context, const message::GetChatServerReq *request,
    message::GetChatServerRsp *response) {
    auto chatServer=choseChatServer();
    response->set_host(chatServer.host);
    response->set_port(chatServer.port);
    response->set_error(ErrorCodes::SUCCESS);
    response->set_token(generateUUID());
    insertToken(request->uid(),response->token());
    return Service::GetChatServer(context, request, response);
}

ChatServer StatusServiceImpl::choseChatServer() {
    std::lock_guard<std::mutex> lock(_serverMutex);
    for (auto &i:_chatServers) {
        if (i.second.connectCount<_freeServer.connectCount) {
            _freeServer=i.second;
        }
    }
    return _freeServer;
}

void StatusServiceImpl::insertToken(int uid, std::string token) {
    std::lock_guard<std::mutex> lock(_tokenMutex);
    _tokens.emplace(uid,token);
}
