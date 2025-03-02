//
// Created by Beyond on 2024/11/3.
//
#include "LogicSystem.h"
#include "CSession.h"
#include <boost/uuid.hpp>
#include "LogicNode.h"

CSession::CSession(boost::asio::io_context &ioc) : _socket(ioc), _isStop(false), _isParsed(false),_msg_body(std::make_shared<MsgNode>()),_msg_head(std::make_shared<MsgNode>(0,4,nullptr)) {
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
    auto self=shared_from_this();
    //读取并处理请求
    _socket.async_read_some(&boost::asio::buffer(_buffer,MSG_MAX_LEN), [self](boost::system::error_code ec, std::size_t bytes_transferred) {
        try {
            if (ec.value()!=0) {
                std::cerr << "read error:" << ec.what() << std::endl;
                return;
            }
            while (bytes_transferred>0) {
                //读取头部
                if (self->_isParsed == false) {
                    if (bytes_transferred+self->_msg_head->currentLen < MSG_HEAD_SIZE) {
                        memcpy(self->_msg_head->data+self->_msg_head->currentLen,self->_buffer+self->_msg_head->currentLen,bytes_transferred);
                        self->_msg_head->currentLen+=bytes_transferred;
                        return;
                    }
                    memcpy(self->_msg_head->data+self->_msg_head->currentLen,self->_buffer+self->_msg_head->currentLen,MSG_HEAD_SIZE-self->_msg_head->currentLen);
                    self->_msg_head->currentLen=MSG_HEAD_SIZE;
                    bytes_transferred-=MSG_HEAD_SIZE;
                    self->_isParsed==true;

                    //解析头部信息
                    memcpy(&self->_msg_head->totalLen,self->_msg_head->data+2,2);
                }
                //读取body
                if (bytes_transferred+self->_msg_body->currentLen<self->_msg_head->totalLen) {
                    memcpy(self->_msg_body->data+self->_msg_body->currentLen,self->_buffer+self->_msg_body->currentLen,bytes_transferred);
                    self->_msg_body->currentLen+=bytes_transferred;
                    return;
                }
                memcpy(self->_msg_body->data+self->_msg_body->currentLen,self->_buffer+self->_msg_body->currentLen,self->_msg_head->totalLen-self->_msg_body->currentLen);
                self->_msg_body->currentLen=self->_msg_body->totalLen;
                bytes_transferred-=self->_msg_head->totalLen;
                //传给逻辑系统队列
                short id;
                memcpy(&id,self->_msg_head->data,2);//获取id
                std::shared_ptr<LogicNode> logicNode =std::make_shared<LogicNode>(id,self->_msg_body,self);
                LogicSystem::getInstance()->postMsgToQueue(logicNode);
                self->_isParsed=false;
            }
            self->read();
        } catch (std::exception &e) {
            std::cerr << "read error:" << e.what() << std::endl;
            self->read();
        }
    });
}
