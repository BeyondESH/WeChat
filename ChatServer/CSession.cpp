//
// Created by Beyond on 2024/11/3.
//
#include "LogicSystem.h"
#include "CSession.h"
#include <boost/uuid.hpp>
CSession::CSession(boost::asio::io_context &ioc) : _socket(ioc) {
    boost::uuids::uuid uuid=boost::uuids::random_generator()();
    _connectionId=boost::uuids::to_string(uuid);
}

void CSession::start() {
    auto self = shared_from_this();
    //读取并处理请求
    _socket.async_read_some(&_buffer,[self](boost::system::error_code ec,std::size_t number) {

    });
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

// void CSession::checkDeadLine() {
//     auto self = shared_from_this();
//     _deadLine.async_wait([self](boost::beast::error_code ec) {
//         if (!ec) {
//             self->_socket.close(ec);
//         }
//     });
// }

//发送应答
void CSession::response() {
    auto self = shared_from_this();
    _response.content_length(_response.body().size());
    boost::beast::http::async_write(_socket, _response,
                                    [self](boost::beast::error_code ec, std::size_t bytes_transferred) {
                                        self->_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
                                        self->_deadLine.cancel();
                                    });
}

//处理请求
void CSession::handleRequest() {
    //设置响应头
    _response.version(_request.version());//设置版本
    _response.keep_alive(false);//短连接
    switch (_request.method()) {
        case boost::beast::http::verb::get: {
            bool result = LogicSystem::getInstance()->handleGet(_request.target(), shared_from_this());
            //处理失败
            if (result==false) {
                _response.result(boost::beast::http::status::not_found);//404
                _response.set(boost::beast::http::field::content_type, "text/plain");
                boost::beast::ostream(_response.body()) << "url nor found\r\n";
                response();//发送
                return;
            }
            //处理成功
            _response.result(boost::beast::http::status::ok);
            _response.set(boost::beast::http::field::server, "GateServer");
            response();//发送
            return;
        }
        case boost::beast::http::verb::post:{
            bool result = LogicSystem::getInstance()->handlePost(_request.target(), shared_from_this());
            //处理失败
            if (result==false) {
                _response.result(boost::beast::http::status::not_found);//404
                _response.set(boost::beast::http::field::content_type, "text/plain");
                boost::beast::ostream(_response.body()) << "url nor found\r\n";
                response();//发送
                return;
            }
            //处理成功
            _response.result(boost::beast::http::status::ok);
            _response.set(boost::beast::http::field::server, "GateServer");
            response();//发送
            return;
        }
        default:
            return;
    }
}

boost::asio::ip::tcp::socket & CSession::getSocket() {
    return this->_socket;
}

std::string & CSession::getSessionId() {
    return _sessionId;
}
