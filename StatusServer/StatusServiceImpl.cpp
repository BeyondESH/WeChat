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

StatusServiceImpl::StatusServiceImpl():index(0) {
    auto &configMgr=ConfigMgr::getInstance();
    ChatServer chatServer;
    chatServer.host=configMgr["ChatServer1"]["host"];
    chatServer.port=configMgr["ChatServer1"]["port"];
    _chatServers.emplace_back(chatServer);
    chatServer.host=configMgr["ChatServer2"]["host"];
    chatServer.port=configMgr["ChatServer2"]["port"];
    _chatServers.emplace_back(chatServer);
}

grpc::Status StatusServiceImpl::GetChatServer(grpc::ServerContext *context, const message::GetChatServerReq *request,
    message::GetChatServerRsp *response) {
    index=(index++)%_chatServers.size();
    auto chatServer=_chatServers[index];
    response->set_host(chatServer.host);
    response->set_port(chatServer.port);
    response->set_error(ErrorCodes::SUCCESS);
    response->set_token(generateUUID());
    return Service::GetChatServer(context, request, response);
}
