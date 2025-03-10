#include <iostream>
#include "LogicSystem.h"
#include "AsioIOcontextPool.h"
#include <thread>
#include <csignal>
#include "ConfigMgr.hpp"
#include "CServer.h"
#include <boost/asio.hpp>
bool isStop = false;
int main()
{
    try
    {
        auto pool = AsioIOcontextPool::getInstance();
        boost::asio::io_context io_context;
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&io_context, pool](const boost::system::error_code &, int)
                           {
            io_context.stop();
            pool->stop(); });
        auto &configMgr = ConfigMgr::getInstance();
        unsigned short port = std::stoul(configMgr["ChatServer1"]["port"]);
        // 使用std::make_shared创建CServer实例，而不是在栈上创建
        auto server = std::make_shared<CServer>(io_context, port);
        server->start();
        io_context.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "main error:" << e.what() << std::endl;
    }
    return 0;
}