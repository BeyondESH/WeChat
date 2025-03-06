//
// Created by Beyond on 2024/11/3.
//
#include "LogicSystem.h"
#include "CSession.h"
#include <boost/uuid.hpp>
#include "LogicNode.h"
#include "CServer.h"

CSession::CSession(boost::asio::io_context &ioc,std::shared_ptr<CServer> cserver) : _cserver(cserver),_socket(ioc), _isStop(false), _isParsed(false),_msg_body(std::make_shared<MsgNode>()),_msg_head(std::make_shared<MsgNode>(0,4,nullptr)) {
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
                self->read();
                return;
            }
            while (bytes_transferred>0) {
                //读取头部
                if (self->_isParsed == false) {
                    self->_msg_head->clear();
                    if (bytes_transferred+self->_msg_head->currentLen < MSG_HEAD_SIZE) {
                        memcpy(self->_msg_head->data+self->_msg_head->currentLen,self->_buffer+self->_msg_head->currentLen,bytes_transferred);
                        self->_msg_head->currentLen+=bytes_transferred;
                        self->read();
                        return;
                    }
                    memcpy(self->_msg_head->data+self->_msg_head->currentLen,self->_buffer+self->_msg_head->currentLen,MSG_HEAD_SIZE-self->_msg_head->currentLen);
                    self->_msg_head->currentLen=MSG_HEAD_SIZE;
                    bytes_transferred-=MSG_HEAD_SIZE;
                    self->_isParsed=true;

                    //解析头部信息
                    memcpy(&self->_msg_body->totalLen,self->_msg_head->data+2,2);
                    self->_msg_body->totalLen=boost::asio::detail::socket_ops::network_to_host_short(self->_msg_body->totalLen);
                    if (self->_msg_body->totalLen>MSG_MAX_LEN) {
                        std::cerr<<"msg body length is invalid:"<<self->_msg_body->totalLen<<std::endl;
                        self->_cserver->closeSession(self->getSessionId());
                        return;
                    }
                }
                //读取body
                self->_msg_body->clear();
                if (bytes_transferred+self->_msg_body->currentLen<self->_msg_body->totalLen) {
                    memcpy(self->_msg_body->data+self->_msg_body->currentLen,self->_buffer+self->_msg_body->currentLen,bytes_transferred);
                    self->_msg_body->currentLen+=bytes_transferred;
                    self->read();
                    return;
                }
                memcpy(self->_msg_body->data+self->_msg_body->currentLen,self->_buffer+self->_msg_body->currentLen,self->_msg_body->totalLen-self->_msg_body->currentLen);
                self->_msg_body->currentLen=self->_msg_body->totalLen;
                bytes_transferred-=self->_msg_head->totalLen;
                //传给逻辑系统队列
                short id;
                memcpy(&id,self->_msg_head->data,2);//获取id
                id=boost::asio::detail::socket_ops::network_to_host_short(id);
                if (id>MSG_MAX_LEN) {
                    std::cerr<<"id is invalid:"<<id<<std::endl;
                    self->_cserver->closeSession(self->getSessionId());
                    return;
                }
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
