//
// Created by Beyond on 2024/11/3.
//

#ifndef GATESERVER_HTTPCONNECTION_H
#define GATESERVER_HTTPCONNECTION_H

#include "const.h"

//提前声明
class LogicSystem;

class HttpConnection:public std::enable_shared_from_this<HttpConnection>{
    friend class LogicSystem;
public:
    HttpConnection(boost::asio::io_context &ioc);
    void start();
    boost::asio::ip::tcp::socket& getSocket();
private:
    boost::asio::ip::tcp::socket _socket;
    boost::beast::flat_buffer _buffer{8192};
    boost::beast::http::request<boost::beast::http::dynamic_body> _request;//接收的请求
    boost::beast::http::response<boost::beast::http::dynamic_body> _response;//发送的应答
    boost::beast::net::steady_timer _deadLine{_socket.get_executor(),std::chrono::seconds(60)};//超时定时器,60s

    void checkDeadLine();//检查超时
    void response();//应答
    void handleRequest();//处理请求
};


#endif //GATESERVER_HTTPCONNECTION_H
