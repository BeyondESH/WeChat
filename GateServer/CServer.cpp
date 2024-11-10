//
// Created by Beyond on 2024/11/3.
//

#include "CServer.h"
#include "HttpConnection.h"

CServer::CServer(boost::asio::io_context &ioc, unsigned short &port)
: _ioc(ioc), _acceptor(_ioc,
                               boost::asio::ip::tcp::endpoint(
                                       boost::asio::ip::address_v4::any(),
                                       port)),
          _socket(_ioc) {
}

void CServer::start() {
    auto self=shared_from_this();
    _acceptor.async_accept(_socket,[self](boost::system::error_code ec){
        try{
            //监听错误，重新监听
            if(ec.value()!=0){
                self->start();
                return;
            }

            //创建连接
            std::make_shared<HttpConnection>(std::move(self->_socket))->start();
            //继续监听
            self->start();
        }catch(std::exception& e){
            //监听错误
            std::cerr<<"acceptor error:"<<e.what()<<std::endl;
            self->start();
        }
    });
}