//
// Created by Beyond on 2024/11/3.
//
#include "LogicSystem.h"
#include "CSession.h"
#include <boost/uuid.hpp>
#include "LogicNode.h"
#include "CServer.h"
#include "SendNode.h"

CSession::CSession(boost::asio::io_context &ioc, std::shared_ptr<CServer> cserver) : _cserver(cserver), _socket(ioc),
    _isStop(false), _isParsed(false), _msg_head(std::make_shared<MsgNode>(0, 4)),
    _msg_body(std::make_shared<MsgNode>(0, 0)) {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    _sessionId = boost::uuids::to_string(uuid);
}

void CSession::start() {
    auto self = shared_from_this();
    read();
}

// void CSession::checkDeadLine() {
//     auto self = shared_from_this();
//     _deadLine.async_wait([self](boost::beast::error_code ec) {
//         if (!ec) {
//             self->_socket.close(ec);
//         }
//     });
// }

boost::asio::ip::tcp::socket &CSession::getSocket() {
    return this->_socket;
}

std::string &CSession::getSessionId() {
    return _sessionId;
}

void CSession::read() {
    auto self = shared_from_this();
    // 读取并处理请求
    _socket.async_read_some(boost::asio::buffer(_buffer, MSG_MAX_LEN),
                            [self](boost::system::error_code ec, std::size_t bytes_transferred) {
                                try {
                                    if (ec.value() != 0) {
                                        std::cerr << "read error:" << ec.what() << std::endl;
                                        if (ec == boost::asio::error::eof || ec ==
                                            boost::asio::error::connection_reset) {
                                            // 连接已关闭或重置
                                            self->_cserver->closeSession(self->getSessionId());
                                            return;
                                        }
                                        self->read();
                                        return;
                                    }

                                    if (bytes_transferred == 0) {
                                        self->read();
                                        return;
                                    }

                                    // 记录当前处理的缓冲区位置
                                    size_t buffer_pos = 0;

                                    while (buffer_pos < bytes_transferred) {
                                        //读取头部
                                        if (self->_isParsed == false) {
                                            // 计算还需要读取多少字节的头部数据
                                            size_t head_remaining = MSG_HEAD_SIZE - self->_msg_head->currentLen;
                                            // 计算本次可以读取的头部字节数
                                            size_t head_to_copy = std::min(
                                                head_remaining, bytes_transferred - buffer_pos);

                                            // 从正确的缓冲区位置拷贝数据
                                            memcpy(self->_msg_head->data + self->_msg_head->currentLen,
                                                   self->_buffer + buffer_pos,
                                                   head_to_copy);

                                            self->_msg_head->currentLen += head_to_copy;
                                            buffer_pos += head_to_copy;

                                            // 如果头部还没读取完整，等待下次读取
                                            if (self->_msg_head->currentLen < MSG_HEAD_SIZE) {
                                                self->read();
                                                return;
                                            }

                                            //解析头部信息,获取body长度
                                            memcpy(&self->_msg_body->totalLen, self->_msg_head->data + 2, 2);
                                            self->_msg_body->totalLen =
                                                    boost::asio::detail::socket_ops::network_to_host_short(
                                                        self->_msg_body->totalLen);
                                            if (self->_msg_body->totalLen > MSG_MAX_LEN) {
                                                std::cerr << "msg body length is invalid:" << self->_msg_body->totalLen
                                                        << std::endl;
                                                self->_cserver->closeSession(self->getSessionId());
                                                return;
                                            }
                                            self->_msg_body = std::make_shared<MsgNode>(0, self->_msg_body->totalLen);
                                            self->_isParsed = true;
                                        }

                                        //读取body
                                        if (self->_isParsed) {
                                            // 计算还需要读取多少字节的消息体
                                            size_t body_remaining =
                                                    self->_msg_body->totalLen - self->_msg_body->currentLen;
                                            // 计算本次可以读取的消息体字节数
                                            size_t body_to_copy = std::min(
                                                body_remaining, bytes_transferred - buffer_pos);

                                            // 从正确的缓冲区位置拷贝数据
                                            memcpy(self->_msg_body->data + self->_msg_body->currentLen,
                                                   self->_buffer + buffer_pos,
                                                   body_to_copy);

                                            self->_msg_body->currentLen += body_to_copy;
                                            buffer_pos += body_to_copy;

                                            // 如果消息体还没读取完整，等待下次读取
                                            if (self->_msg_body->currentLen < self->_msg_body->totalLen) {
                                                self->read();
                                                return;
                                            }

                                            // 消息完整，处理消息
                                            short id;
                                            memcpy(&id, self->_msg_head->data, 2); //获取id
                                            id = boost::asio::detail::socket_ops::network_to_host_short(id);
                                            if (id > MSG_MAX_LEN) {
                                                std::cerr << "id is invalid:" << id << std::endl;
                                                self->_cserver->closeSession(self->getSessionId());
                                                return;
                                            }
                                            std::shared_ptr<LogicNode> logicNode = std::make_shared<LogicNode>(
                                                id, self->_msg_body, self);
                                            LogicSystem::getInstance()->postMsgToQueue(logicNode);
                                            std::cout << "receive msg id:" << id << std::endl;
                                            std::cout << "receive msg body length:" << self->_msg_body->totalLen <<
                                                    std::endl;
                                            std::cout << "receive msg body:" << std::string(
                                                self->_msg_body->data, self->_msg_body->totalLen) << std::endl;

                                            // 重置状态，准备处理下一条消息
                                            self->_isParsed = false;
                                            self->_msg_head->clear();
                                            self->_msg_body->clear();
                                        }
                                    }
                                    // 当前缓冲区已处理完毕，继续读取新数据
                                    self->read();
                                } catch (std::exception &e) {
                                    std::cerr << "read error:" << e.what() << std::endl;
                                    self->_isParsed = false; // 发生异常时重置状态
                                    self->_msg_head->clear();
                                    self->_msg_body->clear();
                                    self->read();
                                }
                            });
}

void CSession::send(char *data, short totalLen, short id) {
    try {
        auto self = shared_from_this();
        std::lock_guard<std::mutex> lock(_mutex);
        int queueSize = _send_queue.size();
        if (queueSize > QUEUE_MAX_SIZE) {
            std::cerr << "queue size is overflow:" << queueSize << std::endl;
            return;
        }

        char *msgData=new char[totalLen+4];
        short network_id = htons(id);
        short network_len = htons(totalLen);
        memcpy(msgData,&network_id,2);
        memcpy(msgData+2,&network_len,2);
        memcpy(msgData+4,data,totalLen);
        _send_queue.emplace(std::make_shared<SendNode>(msgData, id, totalLen+4));
        // 等待前序节点发送完成后pop
        if (queueSize > 0) {
            return;
        }
        auto &sendNode = _send_queue.front();
        boost::asio::async_write(_socket, boost::asio::buffer(sendNode->_data, sendNode->_totalLen),
                                 std::bind(&CSession::handle_async_write, self, std::placeholders::_1,
                                           std::placeholders::_2, self));
    } catch (const boost::system::error_code &ec) {
        std::cerr << "send error:" << ec.what() << std::endl;
        close();
        _cserver->closeSession(getSessionId());
    }
}

void CSession::close() {
    _socket.close();
    _isStop = true;
}

void CSession::handle_async_write(const boost::system::error_code &ec, std::size_t bytes_transferred,
                                  std::shared_ptr<CSession> self) {
    try {
        std::lock_guard<std::mutex> lock(self->_mutex);
        if (ec.value() != 0) {
            std::cerr << "send error:" << ec.what() << std::endl;
            self->close();
            self->_cserver->closeSession(self->getSessionId());
            return;
        }
        self->_send_queue.pop();
        if (!_send_queue.empty()) {
            auto &sendNode = self->_send_queue.front();
            boost::asio::async_write(_socket, boost::asio::buffer(sendNode->_data, sendNode->_totalLen),
                                     std::bind(&CSession::handle_async_write, self, std::placeholders::_1,
                                               std::placeholders::_2, self));
        }
    } catch (const boost::system::error_code &ec) {
        std::cerr << "handle async write error:" << ec.what() << std::endl;
        close();
        _cserver->closeSession(getSessionId());
    }
}
