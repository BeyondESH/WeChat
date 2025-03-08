//
// Created by Beyond on 2024/11/3.
//

#ifndef GATESERVER_HTTPCONNECTION_H
#define GATESERVER_HTTPCONNECTION_H

#include "const.h"
#include <mutex>
#include <queue>
#include <condition_variable>
#include "MsgNode.h"
//提前声明
class LogicSystem;
class CServer;
class SendNode;

class CSession:public std::enable_shared_from_this<CSession>{
    friend class LogicSystem;
public:
    CSession(boost::asio::io_context &ioc,std::shared_ptr<CServer> cserver);
    void start();
    boost::asio::ip::tcp::socket& getSocket();
    std::string& getSessionId();
private:
    void read();
    void send(char *data,short totalLen,short id);
    void close();
    void handle_async_write(const boost::system::error_code &ec, std::size_t bytes_transferred,std::shared_ptr<CSession> self);
    boost::asio::ip::tcp::socket _socket;
    char _buffer[MSG_MAX_LEN];
    //boost::beast::net::steady_timer _deadLine{_socket.get_executor(),std::chrono::seconds(60)};//超时定时器,60s
    //void checkDeadLine();//检查超时
    std::string _sessionId;
    std::string _userId;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::atomic_bool _isStop;
    std::atomic_bool _isParsed;
    std::shared_ptr<MsgNode> _msg_head;
    std::shared_ptr<MsgNode> _msg_body;
    std::shared_ptr<CServer> _cserver;
    std::queue<std::shared_ptr<SendNode>> _send_queue;
};


#endif //GATESERVER_HTTPCONNECTION_H
