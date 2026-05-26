#ifndef CHATSERVER_CSESSION_H
#define CHATSERVER_CSESSION_H

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>
#include "const.h"
#include <mutex>
#include <queue>
#include <chrono>

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

    int getUid() const { return _uid.load(); }
    bool isAuthenticated() const { return _isAuthenticated.load(); }

private:
    void readHead();          
    void readBody();          
    void handleMessage(short msgId, const std::string &msgBody);

    void handle_async_write(const boost::system::error_code &ec, std::size_t bytes_transferred, std::shared_ptr<CSession> self);

    std::shared_ptr<CServer> _cserver;
    std::string _sessionId;

    boost::asio::ip::tcp::socket _socket;
    bool _isStop;

    std::mutex _mutex;
    std::queue<std::shared_ptr<SendNode>> _send_queue;

    char _headBuf[MSG_HEAD_SIZE];
    std::vector<char> _bodyBuf;
    short _msgId = 0;
    short _msgLen = 0;

    std::atomic<bool> _isAuthenticated{false};
    std::atomic<int> _uid{0};
    std::chrono::steady_clock::time_point _lastActiveTime;

    void sendInitialData();
};

#endif //CHATSERVER_CSESSION_H