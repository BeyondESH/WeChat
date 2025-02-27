//
// Created by Beyond on 2024/11/17.
//

#ifndef GATESERVER_VERIFYGRPCCLIENT_H
#define GATESERVER_VERIFYGRPCCLIENT_H

#include <grpcpp/grpcpp.h>
#include <queue>
#include "message.grpc.pb.h"
#include "const.h"
#include "singleton.hpp"

using grpc::Channel;//endpoint
using grpc::ClientContext;//上下文
using grpc::Status;
using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

class RPCStubPool {
public:
    RPCStubPool(std::size_t size,std::string host,std::string port);
    ~RPCStubPool();
    void close();
    std::unique_ptr<VerifyService::Stub> getStub();//获取通信代理
    void returnStub(std::unique_ptr<VerifyService::Stub> stub);//归还通信代理
private:
    std::atomic<bool> _isStop;
    std::size_t _size;
    std::string _port;
    std::string _host;
    std::queue<std::unique_ptr<VerifyService::Stub>> _stubs;
    std::condition_variable _cond;
    std::mutex _mutex;
};


class VerifyGrpcClient :public Singleton<VerifyGrpcClient>{
    friend class Singleton<VerifyGrpcClient>;
public:
    GetVerifyRsp GetVerifyCode(const std::string &email);
private:
    VerifyGrpcClient();
    VerifyGrpcClient(const VerifyGrpcClient&)=delete;
    VerifyGrpcClient&operator=(const VerifyGrpcClient&)=delete;
    std::unique_ptr<RPCStubPool> _pool;
};


#endif //GATESERVER_VERIFYGRPCCLIENT_H
