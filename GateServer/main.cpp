#include <iostream>
#include "const.h"
#include "CServer.h"
#include "ConfigMgr.hpp"
#include "Test.hpp"
#include "MySQLMgr.h"
#include "RedisMgr.h"
#include "StatusGrpcClient.h"
#include "VerifyGrpcClient.h"

int main() {
    auto configMgr=ConfigMgr::getInstance();
    std::string gate_port_str=configMgr["GateServer"]["port"];
    unsigned short gate_port=static_cast<unsigned short>(std::stoi(gate_port_str));
    
    try {
        MySQLMgr::getInstance();
        std::cout << "MySQL管理器初始化完成" << std::endl;
        
        RedisMgr::getInstance();
        std::cout << "Redis管理器初始化完成" << std::endl;
        
        StatusGrpcClient::getInstance()->init();
        std::cout << "状态服务客户端初始化完成" << std::endl;
        
        VerifyGrpcClient::getInstance()->init();
        std::cout << "验证服务客户端初始化完成" << std::endl;
        
        LogicSystem::getInstance();
        std::cout << "逻辑系统初始化完成" << std::endl;
        
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
