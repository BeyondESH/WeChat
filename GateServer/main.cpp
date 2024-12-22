#include <iostream>
#include "const.h"
#include "CServer.h"
#include "ConfigMgr.hpp"
#include "Test.hpp"
#include "MySQLMgr.h"

int main() {
    // try {
    //     Test::getInstance()->RedisConnectPool_reconnect();
    // }catch (std::system_error &e){
    //     std::cout<<e.what()<<std::endl;
    // }
    auto configMgr=ConfigMgr::getInstance();
    std::string gate_port_str=configMgr["GateServer"]["port"];
    unsigned short gate_port=static_cast<unsigned short>(std::stoi(gate_port_str));
    try {
        boost::asio::io_context ioc{1};
        unsigned short port = static_cast<unsigned short>(gate_port);
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](boost::system::error_code ec, int signal_number) {
            if (ec) {
                return;
            }
            ioc.stop();
        });
        auto server=std::make_shared<CServer>(ioc, port);
        server->start();
        std::cout<<"服务器启动，端口："<<port<<std::endl;
        ioc.run();
    } catch (boost::system::system_error &e) {
        std::cerr << "server error:" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
