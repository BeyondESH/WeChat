﻿//
// Created by Beyond on 2024/11/3.
//

#ifndef GATESERVER_CSERVER_H
#define GATESERVER_CSERVER_H

#include "const.h"

//提前声明
class HttpConnection;

class CServer :public std::enable_shared_from_this<CServer>{
public:
    CServer(boost::asio::io_context& ioc,unsigned  short& port);//构造函数
    void start();//启动服务器
private:
    boost::asio::ip::tcp::acceptor _acceptor;//接收器
    boost::asio::io_context& _ioc;//io上下文
};


#endif //GATESERVER_CSERVER_H