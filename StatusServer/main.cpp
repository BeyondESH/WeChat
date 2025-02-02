#include <iostream>
#include <nlohmann/json.hpp>
#include "const.h"
#include "ConfigMgr.hpp"
#include "message.grpc.pb.h"
#include "StatusServiceImpl.h"
#include <grpcpp/grpcpp.h>
#include <boost/asio.hpp>
void runServer() {
    auto & configMgr=ConfigMgr::getInstance();
    std::string server_address=configMgr["StatusServer"]["host"]+":"+configMgr["StatusServer"]["port"];
    StatusServiceImpl service;
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address,grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on..." << server_address << std::endl;
    boost::asio::io_context io_context;
    boost::asio::signal_set signals(io_context,SIGINT,SIGTERM);
    signals.async_wait([&server](const boost::system::error_code &error,int signal_number) {
        if (!error) {
            std::cout<<"Shutting Server down..."<<std::endl;
            server->Shutdown();
        }
    });
    std::thread([&io_context]() {
        io_context.run();
    }).detach();
    server->Wait();
    io_context.stop();
}
int main() {
    try {
        runServer();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}