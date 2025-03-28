//
// Created by Beyond on 2024/11/3.
//

#ifndef CHATSERVER_CSESSION_H
#define CHATSERVER_CSESSION_H

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>
#include "const.h"
#include <mutex>
#include <queue>
#include "MsgNode.h"

class CServer;
class LogicNode;
class SendNode;

class CSession : public std::enable_shared_from_this<CSession> {
public:
    explicit CSession(boost::asio::io_context &ioc, std::shared_ptr<CServer> cserver);

    void start();

    boost::asio::ip::tcp::socket &getSocket();

    std::string &getSessionId();

    void send(char *data, short totalLen, short id);

    void close();

private:
    void read();

    void handle_async_write(const boost::system::error_code &ec, std::size_t bytes_transferred, std::shared_ptr<CSession> self);

    std::shared_ptr<CServer> _cserver;
    std::string _sessionId;

    boost::asio::ip::tcp::socket _socket;
    bool _isStop;

    char _buffer[MSG_MAX_LEN];

    std::mutex _mutex;
    std::queue<std::shared_ptr<SendNode>> _send_queue;

    bool _isParsed;
    std::shared_ptr<MsgNode> _msg_head;
    std::shared_ptr<MsgNode> _msg_body;
    
    // 用于新的消息接收实现
    void doRead();
    char _recvBuf[MAX_MSG_LEN];
    short _msgLen = 0;
    short _msgId = 0;
    int _msgOffset = 0;
    
    // 用户认证相关
    bool _isAuthenticated = false;
    int _uid = 0;
    void sendInitialData();
};

#endif //CHATSERVER_CSESSION_H
