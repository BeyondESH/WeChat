//
// Created by Beyond on 2025/2/2.
//

#ifndef STATUSSERVICEIMPL_H
#define STATUSSERVICEIMPL_H
#include <grpcpp/server.h>
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"

struct ChatServer {
    std::string host;
    std::string port;
};

class StatusServiceImpl final :public message::StatusService::Service{
public:
    StatusServiceImpl();
    grpc::Status GetChatServer(grpc::ServerContext *context, const message::GetChatServerReq *request, message::GetChatServerRsp *response) override;
    std::vector<ChatServer> _chatServers;
    int index;
};



#endif //STATUSSERVICEIMPL_H
