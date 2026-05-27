
#include "StatusServiceImpl.h"
#include "ConfigMgr.hpp"
#include "RedisMgr.h"
#include &lt;boost/uuid/uuid.hpp&gt;
#include &lt;boost/uuid/uuid_generators.hpp&gt;
#include &lt;boost/uuid/uuid_io.hpp&gt;
#include &lt;climits&gt;

std::string generateUUID() {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::string uuid_string = boost::uuids::to_string(uuid);
    return uuid_string;
}

StatusServiceImpl::StatusServiceImpl() {
    auto&amp; configMgr = ConfigMgr::getInstance();
    ChatServer chatServer;
    std::string numberServer = configMgr["ChatServer"]["number"];
    int numberServerInt = std::stoi(numberServer);
    std::cout &lt;&lt; "Number server listening on: " &lt;&lt; numberServerInt &lt;&lt; std::endl;
    
    for (int i = 1; i &lt;= numberServerInt; ++i) {
        std::string serverName = "ChatServer" + std::to_string(i);
        chatServer.name = configMgr[serverName]["name"];
        chatServer.host = configMgr[serverName]["host"];
        chatServer.port = configMgr[serverName]["port"];
        chatServer.missedHeartbeats = 0;
        chatServer.lastHeartbeatTime = std::chrono::steady_clock::now();
        chatServer.lastRedisUpdateTime = std::chrono::steady_clock::now();
        chatServers.emplace(chatServer.name, chatServer);
    }
    
    if (!chatServers.empty()) {
        auto it = chatServers.begin();
        freeServer.host = it-&gt;second.host;
        freeServer.port = it-&gt;second.port;
        freeServer.name = it-&gt;second.name;
    }
    
    healthCheckRunning = true;
    healthCheckThread = std::thread(&amp;StatusServiceImpl::startHealthCheck, this);
}

StatusServiceImpl::~StatusServiceImpl() {
    healthCheckRunning = false;
    if (healthCheckThread.joinable()) {
        healthCheckThread.join();
    }
}

grpc::Status StatusServiceImpl::GetChatServer(grpc::ServerContext* context, const message::GetChatServerReq* request, message::GetChatServerRsp* response) {
    std::cout &lt;&lt; "Run GetChatServer..." &lt;&lt; std::endl;
    if (!request) {
        response-&gt;set_error(ErrorCodes::RPC_FAILED);
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Request is null");
    }

    ChatServer selected = choseChatServer();
    response-&gt;set_host(selected.host);
    response-&gt;set_port(selected.port);
    response-&gt;set_error(ErrorCodes::SUCCESS);
    std::cout &lt;&lt; "GetChatServer success, host:" &lt;&lt; selected.host &lt;&lt; " port:" &lt;&lt; selected.port &lt;&lt; std::endl;

    std::string token = generateUUID();
    response-&gt;set_token(token);
    std::cout &lt;&lt; "Token is:" &lt;&lt; token &lt;&lt; std::endl;

    insertToken(request-&gt;uid(), token);

    return grpc::Status::OK;
}

grpc::Status StatusServiceImpl::CheckToken(grpc::ServerContext* context, const message::CheckTokenReq* request, message::CheckTokenRsp* response) {
    int uid = request-&gt;uid();
    std::string token = request-&gt;token();
    bool result = checkToken(uid, token);
    std::cout &lt;&lt; "check result:" &lt;&lt; result &lt;&lt; std::endl;

    if (result) {
        response-&gt;set_error(ErrorCodes::SUCCESS);
        removeToken(uid);
    } else {
        response-&gt;set_error(ErrorCodes::Token_Invalid);
    }
    response-&gt;set_uid(uid);
    response-&gt;set_token(token);
    return grpc::Status::OK;
}

grpc::Status StatusServiceImpl::RegisterChatServer(grpc::ServerContext* context, const message::RegisterChatServerReq* request, message::RegisterChatServerRsp* response) {
    std::lock_guard&lt;std::mutex&gt; lock(serverMutex);

    ChatServer chatServer;
    chatServer.name = request-&gt;name();
    chatServer.host = request-&gt;host();
    chatServer.port = request-&gt;port();
    chatServer.missedHeartbeats = 0;
    chatServer.lastHeartbeatTime = std::chrono::steady_clock::now();
    chatServer.lastRedisUpdateTime = std::chrono::steady_clock::now();

    chatServers[chatServer.name] = chatServer;

    std::cout &lt;&lt; "ChatServer registered: " &lt;&lt; chatServer.name 
              &lt;&lt; " at " &lt;&lt; chatServer.host &lt;&lt; ":" &lt;&lt; chatServer.port &lt;&lt; std::endl;

    response-&gt;set_error(ErrorCodes::SUCCESS);
    return grpc::Status::OK;
}

grpc::Status StatusServiceImpl::Heartbeat(grpc::ServerContext* context, const message::HeartbeatReq* request, message::HeartbeatRsp* response) {
    std::lock_guard&lt;std::mutex&gt; lock(serverMutex);

    std::string serverName = request-&gt;name();

    auto it = chatServers.find(serverName);
    if (it != chatServers.end()) {
        it-&gt;second.missedHeartbeats = 0;
        it-&gt;second.lastHeartbeatTime = std::chrono::steady_clock::now();
        response-&gt;set_error(ErrorCodes::SUCCESS);
    } else {
        response-&gt;set_error(ErrorCodes::SERVER_NOT_FOUND);
    }

    return grpc::Status::OK;
}

ChatServer StatusServiceImpl::choseChatServer() {
    std::lock_guard&lt;std::mutex&gt; lock(serverMutex);

    ChatServer selectedServer = freeServer;
    int minConnections = INT_MAX;

    for (auto&amp; entry : chatServers) {
        std::string key = "chatserver:" + entry.second.name + ":connections";
        std::string countStr;
        int currentCount = 0;

        if (RedisMgr::getInstance()-&gt;get(key, countStr)) {
            currentCount = std::stoi(countStr);
        }

        if (currentCount &lt; minConnections) {
            minConnections = currentCount;
            selectedServer.host = entry.second.host;
            selectedServer.port = entry.second.port;
            selectedServer.name = entry.second.name;
        }
    }

    std::string key = "chatserver:" + selectedServer.name + ":connections";
    RedisMgr::getInstance()-&gt;hIncrBy(key, "count", 1);

    updateRedisUpdateTime(selectedServer.name);

    freeServer = selectedServer;
    return selectedServer;
}

void StatusServiceImpl::updateRedisUpdateTime(const std::string&amp; serverName) {
    auto it = chatServers.find(serverName);
    if (it != chatServers.end()) {
        it-&gt;second.lastRedisUpdateTime = std::chrono::steady_clock::now();
    }
}

void StatusServiceImpl::startHealthCheck() {
    while (healthCheckRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(5));

        std::lock_guard&lt;std::mutex&gt; lock(serverMutex);
        auto now = std::chrono::steady_clock::now();

        for (auto it = chatServers.begin(); it != chatServers.end(); ) {
            auto&amp; serverInfo = it-&gt;second;

            if (serverInfo.missedHeartbeats &gt;= maxMissedHeartbeats) {
                std::cout &lt;&lt; "ChatServer " &lt;&lt; serverInfo.name 
                          &lt;&lt; " missed " &lt;&lt; serverInfo.missedHeartbeats 
                          &lt;&lt; " heartbeats, removing..." &lt;&lt; std::endl;

                std::string key = "chatserver:" + serverInfo.name + ":connections";
                RedisMgr::getInstance()-&gt;del(key);

                it = chatServers.erase(it);
                continue;
            }

            auto redisElapsed = now - serverInfo.lastRedisUpdateTime;
            if (std::chrono::duration_cast&lt;std::chrono::seconds&gt;(redisElapsed).count() &gt; redisUpdateTimeoutSeconds) {
                std::cout &lt;&lt; "Warning: ChatServer " &lt;&lt; serverInfo.name 
                          &lt;&lt; " no Redis update for " &lt;&lt; redisUpdateTimeoutSeconds &lt;&lt; "s" &lt;&lt; std::endl;
            }

            serverInfo.missedHeartbeats++;
            ++it;
        }
    }
}

void StatusServiceImpl::insertToken(int uid, const std::string&amp; token) {
    std::string key = tokenPrefix + std::to_string(uid);
    RedisMgr::getInstance()-&gt;set(key, token, tokenTTL);
}

bool StatusServiceImpl::checkToken(int uid, const std::string&amp; token) {
    std::string key = tokenPrefix + std::to_string(uid);
    std::string storedToken;
    bool exists = RedisMgr::getInstance()-&gt;get(key, storedToken);

    if (!exists) {
        return false;
    }

    return (storedToken == token);
}

void StatusServiceImpl::removeToken(int uid) {
    std::string key = tokenPrefix + std::to_string(uid);
    RedisMgr::getInstance()-&gt;del(key);
}

