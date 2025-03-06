//
// Created by Beyond on 2024/11/3.
//

#include "CServer.h"
#include "CSession.h"
#include "AsioIOcontextPool.h"

CServer::CServer(boost::asio::io_context &ioc, unsigned short &port)
    : _ioc(ioc), _acceptor(ioc,
                           boost::asio::ip::tcp::endpoint(
                               boost::asio::ip::address_v4::any(),
                               port)) {
}

void CServer::start() {
    auto self = shared_from_this();
    auto &io_context = AsioIOcontextPool::getInstance()->getIOContext(); //从线程池中获取一个io_context
    std::shared_ptr<CSession> new_connection = std::make_shared<CSession>(io_context,self); //创建新连接
    _acceptor.async_accept(new_connection->getSocket(), [self,new_connection](boost::system::error_code ec) {
        try {
            //监听错误，重新监听
            if (ec.value() != 0) {
                self->start();
                return;
            }
            new_connection->start(); //开始处理请求
            {
                std::lock_guard<std::mutex> lock(self->_mutex);
                self->_sessions[new_connection->getSessionId()]=new_connection;
            }
            self->start(); //继续监听
        } catch (std::exception &e) {
            //监听错误
            std::cerr << "acceptor error:" << e.what() << std::endl;
            self->start();
        }
    });
}

void CServer::closeSession(const std::string &sessionId) {
    try {
        std::lock_guard<std::mutex> lock(_mutex);
        _sessions.erase(sessionId);
    } catch (const std::exception &e) {
        std::cerr<<"close session error:"<<e.what()<<std::endl;
    }
}
