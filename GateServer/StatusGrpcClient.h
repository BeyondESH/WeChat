//
// Created by Beyond on 2025/1/26.
//

#ifndef STATUSGRPCCLIENT_H
#define STATUSGRPCCLIENT_H

#include "const.h"
#include "singleton.hpp"
#include "ConfigMgr.hpp"
#include "message.grpc.pb.h"
class StatusGrpcClient;

class StatusConnectPool {
    friend class StatusGrpcClient;
private:
    StatusConnectPool(std::string &host,std::string &port,int poolSize);
    ~StatusConnectPool();
    std::atomic_bool _isStop;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::string _host;
    std::string _port;
    int _poolSize;
    std::queue<std::unique_ptr<message::StatusService::Stub>> _Stubs;
    std::unique_ptr<message::StatusService::Stub> getStub();
    void returnStub(std::unique_ptr<message::StatusService::Stub> stub);
    void close();
};

class StatusGrpcClient:Singleton<StatusGrpcClient> {
friend class Singleton<StatusGrpcClient>;
public:

private:
};



#endif //STATUSGRPCCLIENT_H
