
#include &lt;iostream&gt;
#include "LogicSystem.h"
#include "CServer.h"
#include "ConfigMgr.hpp"
#include "MySQLMgr.h"
#include "SessionMgr.h"
#include "StatusGrpcClient.h"
#include "message.grpc.pb.h"
#include &lt;thread&gt;
#include &lt;signal.h&gt;
#include &lt;grpcpp/grpcpp.h&gt;

volatile sig_atomic_t gShutdown = 0;

void signalHandler(int signum) {
    std::cout &lt;&lt; "Received signal: " &lt;&lt; signum &lt;&lt; ", shutting down..." &lt;&lt; std::endl;
    gShutdown = 1;
}

int main(int argc, char* argv[]) {
    try {
        ConfigMgr::getInstance()-&gt;loadConfig(argc &gt; 1 ? argv[1] : "config.ini");
        std::cout &lt;&lt; "Config loaded" &lt;&lt; std::endl;

        MySQLMgr::getInstance();
        std::cout &lt;&lt; "MySQLMgr initialized" &lt;&lt; std::endl;

        RedisMgr::getInstance();
        std::cout &lt;&lt; "RedisMgr initialized" &lt;&lt; std::endl;

        StatusGrpcClient::getInstance()-&gt;init();
        std::cout &lt;&lt; "StatusGrpcClient initialized" &lt;&lt; std::endl;

        SessionMgr::getInstance();
        std::cout &lt;&lt; "SessionMgr initialized" &lt;&lt; std::endl;

        LogicSystem::getInstance();
        std::cout &lt;&lt; "LogicSystem initialized" &lt;&lt; std::endl;

        auto configMgr = ConfigMgr::getInstance();
        std::string serverName = configMgr["ChatServer"]["name"];
        std::string serverHost = configMgr["ChatServer"]["host"];
        std::string serverPort = configMgr["ChatServer"]["port"];
        std::string statusHost = configMgr["StatusServer"]["host"];
        std::string statusPort = configMgr["StatusServer"]["port"];
        std::string statusAddress = statusHost + ":" + statusPort;

        std::cout &lt;&lt; "Registering ChatServer " &lt;&lt; serverName 
                  &lt;&lt; " with StatusServer at " &lt;&lt; statusAddress &lt;&lt; std::endl;

        auto channel = grpc::CreateChannel(statusAddress, grpc::InsecureChannelCredentials());
        auto stub = message::StatusService::NewStub(channel);

        message::RegisterChatServerReq registerReq;
        registerReq.set_name(serverName);
        registerReq.set_host(serverHost);
        registerReq.set_port(serverPort);

        message::RegisterChatServerRsp registerRsp;
        grpc::ClientContext registerCtx;
        grpc::Status registerStatus = stub-&gt;RegisterChatServer(&amp;registerCtx, registerReq, &amp;registerRsp);

        if (registerStatus.ok() &amp;&amp; registerRsp.error() == ErrorCodes::SUCCESS) {
            std::cout &lt;&lt; "ChatServer registered successfully!" &lt;&lt; std::endl;
        } else {
            std::cerr &lt;&lt; "Failed to register ChatServer: " &lt;&lt; registerStatus.error_message() &lt;&lt; std::endl;
            return 1;
        }

        std::atomic&lt;bool&gt; heartbeatRunning(true);
        std::thread heartbeatThread([&amp;]() {
            while (heartbeatRunning) {
                std::this_thread::sleep_for(std::chrono::seconds(5));

                if (!heartbeatRunning) break;

                message::HeartbeatReq heartbeatReq;
                heartbeatReq.set_name(serverName);

                message::HeartbeatRsp heartbeatRsp;
                grpc::ClientContext heartbeatCtx;
                grpc::Status heartbeatStatus = stub-&gt;Heartbeat(&amp;heartbeatCtx, heartbeatReq, &amp;heartbeatRsp);

                if (!heartbeatStatus.ok()) {
                    std::cerr &lt;&lt; "Failed to send heartbeat: " &lt;&lt; heartbeatStatus.error_message() &lt;&lt; std::endl;
                }
            }
        });

        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        CServer server(serverHost, std::stoi(serverPort));
        std::cout &lt;&lt; "ChatServer starting at " &lt;&lt; serverHost &lt;&lt; ":" &lt;&lt; serverPort &lt;&lt; std::endl;

        std::thread serverThread([&amp;server]() {
            server.run();
        });

        while (!gShutdown) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout &lt;&lt; "Stopping ChatServer..." &lt;&lt; std::endl;
        server.stop();

        heartbeatRunning = false;
        if (heartbeatThread.joinable()) {
            heartbeatThread.join();
        }

        if (serverThread.joinable()) {
            serverThread.join();
        }

        LogicSystem::getInstance()-&gt;close();
        MySQLMgr::getInstance()-&gt;close();
        StatusGrpcClient::getInstance()-&gt;close();

        std::cout &lt;&lt; "ChatServer stopped" &lt;&lt; std::endl;
        return 0;
    } catch (const std::exception&amp; e) {
        std::cerr &lt;&lt; "Server failed: " &lt;&lt; e.what() &lt;&lt; std::endl;
        return 1;
    }
}

