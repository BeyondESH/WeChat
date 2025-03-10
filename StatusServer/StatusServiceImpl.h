//
// Created by Beyond on 2025/2/2.
//

#ifndef STATUSSERVICEIMPL_H
#define STATUSSERVICEIMPL_H
#include <grpcpp/server.h>
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include <unordered_map>
#include <mutex>
struct ChatServer {
    std::string host;
    std::string port;
    std::string name;
    int connectCount;
};

class StatusServiceImpl final :public message::StatusService::Service{
public:
    StatusServiceImpl();
    grpc::Status GetChatServer(grpc::ServerContext *context, const message::GetChatServerReq *request, message::GetChatServerRsp *response) override;
    grpc::Status CheckToken(grpc::ServerContext *context, const message::CheckTokenReq *request, message::CheckTokenRsp *response) override;
    ChatServer choseChatServer();
    void insertToken(int uid,std::string token);
    bool checkToken(int &uid,std::string &token);
    std::unordered_map<std::string,ChatServer> _chatServers;
    std::mutex _serverMutex;
    std::unordered_map<int,std::string> _tokens;
    std::mutex _tokenMutex;
    ChatServer _freeServer;
};



#endif //STATUSSERVICEIMPL_H
