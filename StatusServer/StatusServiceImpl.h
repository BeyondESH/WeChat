
#ifndef STATUS_SERVICE_IMPL_H
#define STATUS_SERVICE_IMPL_H

#include &lt;grpcpp/server.h&gt;
#include &lt;grpcpp/grpcpp.h&gt;
#include "message.grpc.pb.h"
#include &lt;unordered_map&gt;
#include &lt;mutex&gt;
#include &lt;chrono&gt;
#include &lt;thread&gt;
#include &lt;atomic&gt;

struct ChatServer {
    std::string host;
    std::string port;
    std::string name;
    int missedHeartbeats;
    std::chrono::steady_clock::time_point lastHeartbeatTime;
    std::chrono::steady_clock::time_point lastRedisUpdateTime;
};

class StatusServiceImpl final : public message::StatusService::Service {
public:
    StatusServiceImpl();
    ~StatusServiceImpl();
    grpc::Status GetChatServer(grpc::ServerContext* context, const message::GetChatServerReq* request, message::GetChatServerRsp* response) override;
    grpc::Status CheckToken(grpc::ServerContext* context, const message::CheckTokenReq* request, message::CheckTokenRsp* response) override;
    grpc::Status RegisterChatServer(grpc::ServerContext* context, const message::RegisterChatServerReq* request, message::RegisterChatServerRsp* response) override;
    grpc::Status Heartbeat(grpc::ServerContext* context, const message::HeartbeatReq* request, message::HeartbeatRsp* response) override;
    ChatServer choseChatServer();
    void insertToken(int uid, const std::string&amp; token);
    bool checkToken(int uid, const std::string&amp; token);
    void removeToken(int uid);

private:
    std::unordered_map&lt;std::string, ChatServer&gt; chatServers;
    std::mutex serverMutex;
    ChatServer freeServer;

    const std::string tokenPrefix = "token_";
    const int tokenTTL = 300;
    const int maxMissedHeartbeats = 5;
    const int redisUpdateTimeoutSeconds = 60;

    void updateRedisUpdateTime(const std::string&amp; serverName);
    void startHealthCheck();
    std::atomic&lt;bool&gt; healthCheckRunning;
    std::thread healthCheckThread;
};

#endif //STATUS_SERVICE_IMPL_H

