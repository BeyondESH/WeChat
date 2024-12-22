//
// Created by Beyond on 2024/11/17.
//

#include "VerifyGrpcClient.h"
#include "ConfigMgr.hpp"

GetVerifyRsp VerifyGrpcClient::GetVerifyCode(const std::string &email) {
    ClientContext context;//上下文
    GetVerifyRsp reply;//grpc回包
    GetVerifyReq request;//grpc请求
    request.set_email(email);//设置请求参数
    auto stub=_pool->getStub();//从代理池获取通信代理
    Status status = stub->GetVerifyCode(&context, request, &reply);//获取验证码
    if (status.ok()) {
        _pool->returnStub(std::move(stub));//归还通信代理
        reply.set_error(ErrorCodes::SUCCESS);
        return reply;
    } else {
        _pool->returnStub(std::move(stub));//归还通信代理
        reply.set_error(ErrorCodes::RPC_FAILED);
        return reply;
    }
}

VerifyGrpcClient::VerifyGrpcClient() {
    auto &configMgr=ConfigMgr::getInstance();
    std::string host=configMgr["VerifyServer"]["host"];
    std::string port=configMgr["VerifyServer"]["port"];
    _pool=std::make_unique<RPCStubPool>(std::thread::hardware_concurrency(),host,port);
}

RPCStubPool::RPCStubPool(std::size_t size,std::string host, std::string port):_size(size),_host(host),_port(port),_isStop(false) {
    for (int i = 0; i < size; ++i) {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host+":"+port, grpc::InsecureChannelCredentials());//创建通道
        _stubs.push(VerifyService::NewStub(channel));//创建通信代理 参数为临时变量的右值，内部调用移动构造函数
    }
}

RPCStubPool::~RPCStubPool() {
    std::lock_guard<std::mutex> lock(_mutex);
    close();
    while (!_stubs.empty()) {
        _stubs.pop();
    }
}

void RPCStubPool::close() {
    _isStop=true;
    _cond.notify_all();//通知所有线程完成剩下的工作
}

std::unique_ptr<VerifyService::Stub> RPCStubPool::getStub() {
    if (_isStop) {
        return nullptr;
    }//如果连接池已经关闭，返回空指针,不会进入条件变量
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock,[this]() {
        if (_isStop==true) {
            return true;
        } else {
            return !_stubs.empty();
        }
    });
    if (_isStop) {
        return nullptr;
    }//在wait的过程中，若连接池关闭，返回空指针
    auto stub = std::move(_stubs.front());
    _stubs.pop();
    return stub;
}

void RPCStubPool::returnStub(std::unique_ptr<VerifyService::Stub> stub) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_isStop) {
        return;
    }
    _stubs.push(std::move(stub));
    _cond.notify_one();
}
