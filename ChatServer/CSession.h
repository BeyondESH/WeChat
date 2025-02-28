//
// Created by Beyond on 2024/11/3.
//

#ifndef GATESERVER_HTTPCONNECTION_H
#define GATESERVER_HTTPCONNECTION_H

#include "const.h"
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
//提前声明
class LogicSystem;

class CSession:public std::enable_shared_from_this<CSession>{
    friend class LogicSystem;
public:
    CSession(boost::asio::io_context &ioc);
    void start();
    boost::asio::ip::tcp::socket& getSocket();
    std::string& getSessionId();
private:
    boost::asio::ip::tcp::socket _socket;
    boost::beast::flat_buffer _buffer{8192};

    //boost::beast::net::steady_timer _deadLine{_socket.get_executor(),std::chrono::seconds(60)};//超时定时器,60s
    //void checkDeadLine();//检查超时

    std::string _sessionId;
    std::string _uid;
    std::mutex _mutex;
    int _cv;
};


#endif //GATESERVER_HTTPCONNECTION_H
