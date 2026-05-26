//
// Created by Beyond on 2024/11/3.
//
#include "LogicSystem.h"
#include "HttpConnection.h"

HttpConnection::HttpConnection(boost::asio::io_context &ioc) : _socket(ioc) {
}

void HttpConnection::start() {
    auto self = shared_from_this();
    //读取并处理请求
    boost::beast::http::async_read(_socket, _buffer, _request,
                                   [self](boost::beast::error_code ec, std::size_t bytes_transferred) {
                                       try {
                                           //读取错误
                                           if (ec.value() != 0) {
                                               std::cerr << "read error:" << ec.what() << std::endl;
                                               return;
                                           }
                                           boost::ignore_unused(bytes_transferred);//不需要粘包处理
                                           self->handleRequest();//处理请求
                                           self->checkDeadLine();//检查超时
                                       } catch (std::exception &e) {
                                           std::cerr << "read error:" << e.what() << std::endl;
                                       }
                                   });
}

void HttpConnection::checkDeadLine() {
    auto self = shared_from_this();
    _deadLine.async_wait([self](boost::beast::error_code ec) {
        if (!ec) {
            self->_socket.close(ec);
        }
    });
}

//发送应答
void HttpConnection::response() {
    auto self = shared_from_this();
    _response.content_length(_response.body().size());
    boost::beast::http::async_write(_socket, _response,
                                    [self](boost::beast::error_code ec, std::size_t bytes_transferred) {
                                        self->_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
                                        self->_deadLine.cancel();
                                    });
}

//处理请求
void HttpConnection::handleRequest() {
    //设置响应头
    _response.version(_request.version());//设置版本
    _response.keep_alive(false);//短连接
    switch (_request.method()) {
        case boost::beast::http::verb::get: {
            LogicSystem::getInstance()->pushTask(std::string(_request.target()), shared_from_this(), true);
            return;
        }
        case boost::beast::http::verb::post:{
            LogicSystem::getInstance()->pushTask(std::string(_request.target()), shared_from_this(), false);
            return;
        }
        default:
            return;
    }
}

boost::asio::ip::tcp::socket & HttpConnection::getSocket() {
    return this->_socket;
}
