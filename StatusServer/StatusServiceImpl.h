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

class StatusServiceImpl final : public message::StatusService::Service {
public:
    StatusServiceImpl();
    grpc::Status GetChatServer(grpc::ServerContext *context, const message::GetChatServerReq *request, message::GetChatServerRsp *response) override;
    grpc::Status CheckToken(grpc::ServerContext *context, const message::CheckTokenReq *request, message::CheckTokenRsp *response) override;
    ChatServer choseChatServer();
    void insertToken(int uid, const std::string& token);
    bool checkToken(int uid, const std::string& token);
    void removeToken(int uid);

private:
    std::unordered_map<std::string, ChatServer> _chatServers;
    std::mutex _serverMutex;
    ChatServer _freeServer;

    const std::string TOKEN_PREFIX = "token_";
    const int TOKEN_TTL = 300;
};

#endif //STATUSSERVICEIMPL_H