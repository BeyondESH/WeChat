#include <iostream>
#include "LogicSystem.h"
#include "CServer.h"
#include "ConfigMgr.hpp"
#include "MySQLMgr.h"
#include "SessionMgr.h"
#include "StatusGrpcClient.h"
#include <thread>
#include <signal.h>

// 优雅关闭服务器的信号处理
volatile sig_atomic_t g_shutdown = 0;

void signal_handler(int signum) {
    std::cout << "接收到信号: " << signum << ", 准备关闭服务器..." << std::endl;
    g_shutdown = 1;
}

int main(int argc, char *argv[]) {
    // 注册信号处理函数
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    try {
        // 加载配置
        ConfigMgr::getInstance()->loadConfig(argc > 1 ? argv[1] : "config.ini");
        std::cout << "配置加载完成" << std::endl;
        
        // 初始化组件
        MySQLMgr::getInstance();
        std::cout << "MySQL管理器初始化完成" << std::endl;
        
        StatusGrpcClient::getInstance()->init();
        std::cout << "状态服务客户端初始化完成" << std::endl;
        
        SessionMgr::getInstance();
        std::cout << "会话管理器初始化完成" << std::endl;
        
        LogicSystem::getInstance();
        std::cout << "逻辑系统初始化完成" << std::endl;
        
        // 启动服务器
        auto configMgr = ConfigMgr::getInstance();
        auto host = configMgr["ChatServer"]["host"];
        auto port = std::stoi(configMgr["ChatServer"]["port"]);
        
        CServer server(host, port);
        std::cout << "聊天服务器正在启动，监听: " << host << ":" << port << std::endl;
        
        // 启动服务器
        std::thread serverThread([&server]() {
            server.run();
        });
        
        // 主线程等待关闭信号
        while (!g_shutdown) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        // 收到关闭信号，停止服务器
        std::cout << "正在关闭聊天服务器..." << std::endl;
        server.stop();
        
        if (serverThread.joinable()) {
            serverThread.join();
        }
        
        // 关闭组件
        LogicSystem::getInstance()->close();
        MySQLMgr::getInstance()->close();
        StatusGrpcClient::getInstance()->close();
        
        std::cout << "聊天服务器已成功关闭" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "服务器启动失败: " << e.what() << std::endl;
        return 1;
    }
}