//
// Created by Beyond on 2025/1/26.
//

#include "StatusGrpcClient.h"

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
