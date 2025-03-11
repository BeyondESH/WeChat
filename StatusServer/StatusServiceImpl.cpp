//
// Created by Beyond on 2025/2/2.
//

#include "StatusServiceImpl.h"
#include "ConfigMgr.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
std::string generateUUID()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::string uuid_string = boost::uuids::to_string(uuid);
    return uuid_string;
}

StatusServiceImpl::StatusServiceImpl()
{
    auto &configMgr = ConfigMgr::getInstance();
    ChatServer chatServer;
    std::string numberServer=configMgr["ChatServer"]["number"];
    int numberServer_int=stoi(numberServer);
    std::cout<<"Number server listening on: "<<numberServer_int<<std::endl;
    for (int i = 1; i <=numberServer_int; ++i) {
        std::string serverName="ChatServer"+std::to_string(i);
        chatServer.host=configMgr[serverName]["host"];
        chatServer.port=configMgr[serverName]["port"];
        chatServer.name=configMgr[serverName]["name"];
        _chatServers.emplace(chatServer.name,chatServer);
    }
    _freeServer=_chatServers.begin()->second;
}

grpc::Status StatusServiceImpl::GetChatServer(grpc::ServerContext *context, const message::GetChatServerReq *request,
                                              message::GetChatServerRsp *response)
{
    std::cout << "Run GetChatServer..." << std::endl;
    // 检查请求是否合法
    if (!request) {
        response->set_error(ErrorCodes::RPC_FAILED);
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Request is null");
    }
    auto chatServer = choseChatServer();
    response->set_host(chatServer.host);
    response->set_port(chatServer.port);
    response->set_error(ErrorCodes::SUCCESS);
    std::cout << "GetChatServer success, host:"<<chatServer.host<<" port:"<<chatServer.port<< std::endl;
    response->set_token(generateUUID());
    std::cout << "Token is:" << response->token() << std::endl;
    insertToken(request->uid(), response->token());
    return grpc::Status::OK;
}

grpc::Status StatusServiceImpl::CheckToken(grpc::ServerContext *context, const message::CheckTokenReq *request,
    message::CheckTokenRsp *response) {
    int uid =request->uid();
    std::string token =request->token();
    bool result =checkToken(uid,token);
    std::cout<<"check result:"<<result<<std::endl;
    if (result==true) {
        response->set_error(ErrorCodes::SUCCESS);
    }else {
        response->set_error(ErrorCodes::Token_Invalid);
    }
    response->set_uid(uid);
    response->set_token(token);
    return grpc::Status::OK;
}

ChatServer StatusServiceImpl::choseChatServer()
{
    std::lock_guard<std::mutex> lock(_serverMutex);
    for (auto &i : _chatServers)
    {
        if (i.second.connectCount < _freeServer.connectCount)
        {
            _freeServer = i.second;
            i.second.connectCount++;
        }
    }
    return _freeServer;
}

void StatusServiceImpl::insertToken(int uid, std::string token)
{
    std::lock_guard<std::mutex> lock(_tokenMutex);
    _tokens.emplace(uid, token);
}

bool StatusServiceImpl::checkToken(int &uid, std::string &token) {
    auto iter=_tokens.find(uid);
    if (iter==_tokens.end()) {
        return false;
    }
    if (iter->second==token) {
        return true;
    } else {
        return false;
    }
}
