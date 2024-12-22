//
// Created by Beyond on 2024/11/26.
//

#ifndef REDISMGR_H
#define REDISMGR_H

#include "const.h"

class RedisConnectPool {
    friend class RedisMgr;
public:
    RedisConnectPool(const std::string &host,const int &port,const std::string &password,const size_t &poolSize);
    ~RedisConnectPool();
    redisContext* getConnection();
    void returnConnection(redisContext *connect);
    void close();
private:
    size_t _poolSize;
    std::string _host;
    int _port;
    std::string _password;
    std::atomic<bool> _isStop;
    std::queue<redisContext *> _connects;
    std::mutex _mutex;
    std::condition_variable _cond;
    std::once_flag _flag;
    void reconnect();
};

class RedisMgr:public Singleton<RedisMgr> {
    friend class Singleton<RedisMgr>;
public:
    ~RedisMgr();
    bool get(const std::string &key,std::string &value);
    bool hGet(const std::string &key,std::string &field,std::string &value);
    bool set(const std::string &key,const std::string &value);
    bool hSet(const std::string &key,const std::string &field,const std::string &value);
    bool hSet(const char * key,const char *field,const char *value,size_t valueLen);
    bool auth(const std::string & password);
    bool lPush(const std::string &key,const std::string &value);
    bool rPush(const std::string &key,const std::string &value);
    bool lPop(const std::string &key,std::string &value);
    bool rPop(const std::string &key,std::string &value);
    bool del(const std::string &key);
    bool existsKey(const std::string &key);
    void close();
    int connectCount();
private:
    RedisMgr();
    RedisMgr(const RedisMgr &)=delete;
    RedisMgr&operator=(const RedisMgr &)=delete;
    std::unique_ptr<RedisConnectPool> _pool;
    std::string _host;
    int _port;
    std::string _password;
};



#endif //REDISMGR_H
