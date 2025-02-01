//
// Created by Beyond on 2025/1/26.
//

#include "StatusGrpcClient.h"
#include "Defer.hpp"
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>

StatusConnectPool::StatusConnectPool(std::string &host, std::string &port, int poolSize=std::thread::hardware_concurrency()):_poolSize(poolSize),_host(host),_port(port) {
    for (int i = 0; i < _poolSize; ++i) {
        std::shared_ptr<grpc::Channel> channel=grpc::CreateChannel(host+":"+port,grpc::InsecureChannelCredentials());
        _Stubs.emplace(message::StatusService::NewStub(channel));
    }
}

StatusConnectPool::~StatusConnectPool() {
    std::lock_guard<std::mutex> lock(_mutex);
    close();
    while (!_Stubs.empty()) {
        _Stubs.pop();
    }
}

std::unique_ptr<message::StatusService::Stub> StatusConnectPool::getStub() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock,[this]() {
        if (_isStop) {
            return true;
        }
        return !_Stubs.empty();
    });
    if (_isStop) {
        return nullptr;
    }
    auto stub=std::move(_Stubs.front());
    _Stubs.pop();
    return stub;
}

void StatusConnectPool::returnStub(std::unique_ptr<message::StatusService::Stub> stub) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_isStop) {
        return;
    }
    _Stubs.emplace(std::move(stub));
    _cv.notify_one();
    return;
}

void StatusConnectPool::close() {
    _isStop=true;
    _cv.notify_all();
}

StatusGrpcClient::StatusGrpcClient() {
    auto &configMgr=ConfigMgr::getInstance();
    std::string host=configMgr["StatusServer"]["host"];
    std::string port=configMgr["StatusServer"]["port"];
    _pool=std::unique_ptr<StatusConnectPool>(new StatusConnectPool(host,port,std::thread::hardware_concurrency()));
}

message::GetChatServerRsp StatusGrpcClient::getChatServer(const int &uid) {
    try {
        grpc::ClientContext context;
        message::GetChatServerRsp respond;
        message::GetChatServerReq request;
        auto stub=_pool->getStub();
        if (stub==nullptr) {
            return respond;
        }
        Defer defer([&]() {
            _pool->returnStub(std::move(stub));
        });
        grpc::Status status=stub->GetChatServer(&context,request,&respond);
        if (status.ok()) {
            return respond;
        }else {
            respond.set_error(RPC_FAILED);
            return respond;
        }
    } catch (const std::exception &e) {
        std::cerr<<e.what()<<std::endl;
        message::GetChatServerRsp respond;
        respond.set_error(RPC_FAILED);
        return respond;
    }
}
