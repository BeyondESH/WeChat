//
// Created by Beyond on 2024/11/26.
//

#include "RedisMgr.h"
#include "ConfigMgr.hpp"

RedisMgr::RedisMgr() {
    auto configMgr = ConfigMgr::getInstance();
    _host = configMgr["Redis"]["host"];
    _port = std::stoi(configMgr["Redis"]["port"]);
    _password = configMgr["Redis"]["password"];
    _pool = std::make_unique<RedisConnectPool>(_host, _port, _password, std::thread::hardware_concurrency());
}

RedisMgr::~RedisMgr() {
    close();
    std::cout << "RedisMgr is destructed" << std::endl;
}

bool RedisMgr::get(const std::string &key, std::string &value) {
    auto connect = _pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "get %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_STRING) {
        std::cout << "Failed to execute command [ get " << key << " ]" << std::endl;
        freeReplyObject(reply);
        _pool->returnConnection(connect);
        return false;
    }
    value = reply->str;
    freeReplyObject(reply);
    std::cout << "Succeed to execute command [ get " << key << " ]" << std::endl;
    _pool->returnConnection(connect);
    return true;
}

bool RedisMgr::set(const std::string &key, const std::string &value) {
    auto connect = _pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "set %s %s", key.c_str(), value.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_STATUS || strcmp(reply->str, "OK") != 0) {
        std::cout << "Failed to execute command [ set " << key << " " << value << " ] " << std::endl;
        freeReplyObject(reply);
        _pool->returnConnection(connect);
        return false;
    }
    freeReplyObject(reply);
    std::cout << "Succeed to execute command [ set " << key << " " << value << " ]" << std::endl;
    _pool->returnConnection(connect);
    return true;
}

bool RedisMgr::auth(const std::string &password) {
    auto connect = _pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "auth %s", password.c_str());
    if (reply == nullptr || reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Redis auth failure" << std::endl;
        freeReplyObject(reply);
        _pool->returnConnection(connect);
        return false;
    }
    freeReplyObject(reply);
    std::cout << "Redis auth success" << std::endl;
    _pool->returnConnection(connect);
    return true;
}

bool RedisMgr::lPush(const std::string &key, const std::string &value) {
    auto connect = _pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "lpush %s %s", key.c_str(), value.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        std::cout << "Failed to execute command [ lpush " << key << " " << value << " ]" << std::endl;
        freeReplyObject(reply);
        _pool->returnConnection(connect);
        return false;
    }
    std::cout << "Succeed to execute command [ lpush " << key << " " << value << " ]" << std::endl;
    freeReplyObject(reply);
    _pool->returnConnection(connect);
    return true;
}

bool RedisMgr::rPush(const std::string &key, const std::string &value) {
    auto connect = _pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "rpush %s %s", key.c_str(), value.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        std::cout << "Failed to execute command [ rpush " << key << " " << value << " ]" << std::endl;
        freeReplyObject(reply);
        _pool->returnConnection(connect);
        return false;
    }
    std::cout << "Succeed to execute command [ rpush " << key << " " << value << " ]" << std::endl;
    freeReplyObject(reply);
    _pool->returnConnection(connect);
    return true;
}

bool RedisMgr::lPop(const std::string &key, std::string &value) {
    auto connect = _pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "lpop %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_STRING) {
        std::cout << "Failed to execute command [ lpop " << key << " ]" << std::endl;
        freeReplyObject(reply);
        _pool->returnConnection(connect);
        return false;
    }
    value = reply->str;
    std::cout << "Succeed to execute command [ lpop " << key << " ]" << std::endl;
    freeReplyObject(reply);
    _pool->returnConnection(connect);
    return true;
}

bool RedisMgr::rPop(const std::string &key, std::string &value) {
    auto connect = _pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "rpop %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_STRING) {
        std::cout << "Failed to execute command [ rpop " << key << " ]" << std::endl;
        freeReplyObject(reply);
        _pool->returnConnection(connect);
        return false;
    }
    value = reply->str;
    std::cout << "Succeed to execute command [ rpop " << key << " ]" << std::endl;
    freeReplyObject(reply);
    _pool->returnConnection(connect);
    return true;
}

bool RedisMgr::del(const std::string &key) {
    auto connect = _pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "del %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        std::cout << "Failed to execute command [ del " << key << " ]" << std::endl;
        freeReplyObject(reply);
        _pool->returnConnection(connect);
        return false;
    }
    std::cout << "Succeed to execute command [ del " << key << " ]" << std::endl;
    freeReplyObject(reply);
    _pool->returnConnection(connect);
    return true;
}

bool RedisMgr::existsKey(const std::string &key) {
    auto connect = _pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "exists %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        std::cout << "Failed to execute command [ exists " << key << " ] " << std::endl;
        freeReplyObject(reply);
        _pool->returnConnection(connect);
        return false;
    }
    std::cout << "Succeed to execute command [ exists " << key << " ]" << std::endl;
    freeReplyObject(reply);
    _pool->returnConnection(connect);
    return true;
}


bool RedisMgr::hGet(const std::string &key, std::string &field, std::string &value) {
    auto connect = _pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "hget %s %s", key.c_str(), field.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_STRING) {
        std::cout << "Failed to execute command [ hget " << key << " " << field << " ]" << std::endl;
        freeReplyObject(reply);
        _pool->returnConnection(connect);
        return false;
    }
    value = reply->str;
    std::cout << "Succeed to execute command [ hget " << key << " " << field << " ]" << std::endl;
    freeReplyObject(reply);
    _pool->returnConnection(connect);
    return true;
}

bool RedisMgr::hSet(const std::string &key, const std::string &field, const std::string &value) {
    auto connect = _pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    auto reply = (redisReply *) redisCommand(connect, "hset %s %s %s", key.c_str(), field.c_str(), value.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        std::cout << "Failed to execute command [ hset " << key << " " << field << " " << value << " ]" << std::endl;
        freeReplyObject(reply);
        _pool->returnConnection(connect);
        return false;
    }
    std::cout << "Succeed to execute command [ hset " << key << " " << field << " " << value << " ]" << std::endl;
    freeReplyObject(reply);
    _pool->returnConnection(connect);
    return true;
}

bool RedisMgr::hSet(const char *key, const char *field, const char *value, size_t valueLen) {
    auto connect = _pool->getConnection();
    if (connect == nullptr) {
        return false;
    }
    const char *argv[4];
    size_t argvLen[4];
    argv[0] = "hset";
    argvLen[0] = 4;
    argv[1] = key;
    argvLen[1] = strlen(key);
    argv[2] = field;
    argvLen[2] = strlen(field);
    argv[3] = value;
    argvLen[3] = valueLen;
    auto reply = (redisReply *) redisCommandArgv(connect, 4, argv, argvLen);
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        std::cout << "Failed to execute command [ hset " << key << " " << field << " " << value << " ]" << std::endl;
        freeReplyObject(reply);
        _pool->returnConnection(connect);
        return false;
    }
    std::cout << "Succeed to execute command [ hset " << key << " " << field << " " << value << " ]" << std::endl;
    freeReplyObject(reply);
    _pool->returnConnection(connect);
    return true;
}

void RedisMgr::close() {
    _pool->close();
    std::cout << "Close redis connection" << std::endl;
}

RedisConnectPool::RedisConnectPool(const std::string &host, const int &port, const std::string &password,
                                   const size_t &poolSize): _host(host), _password(password), _port(port),
                                                            _poolSize(poolSize), _isStop(false) {
    for (int i = 0; i < _poolSize; i++) {
        auto connect = redisConnect(_host.c_str(), _port); //连接redis
        if (connect == nullptr || connect->err != 0) {
            if (connect != nullptr) {
                redisFree(connect); //释放连接
            }
            continue;
        }
        std::cout << "Connect redisServer success" << std::endl;
        auto reply = (redisReply *) redisCommand(connect, "auth %s", _password.c_str()); //密码验证
        if (reply->type == REDIS_REPLY_ERROR) {
            std::cout << "Redis auth failure" << std::endl;
            freeReplyObject(reply); //释放reply
            continue;
        }
        freeReplyObject(reply); //释放reply
        std::cout << "Redis auth success" << std::endl;
        _mutex.lock();
        _connects.push(connect);
        _mutex.unlock();
    }
    _mutex.lock();
    int size = _connects.size(); //获取连接池中的连接数
    _mutex.unlock();
    if (size != _poolSize) {
        std::thread _allConnect_thread = std::thread([this,size]() {
            while (true) {
                std::cout << "try to reconnect redisServer" << std::endl;
                for (int i = 0; i < _poolSize - size; i++) {
                    auto connect = redisConnect(_host.c_str(), _port); //连接redis
                    if (connect == nullptr || connect->err != 0) {
                        if (connect != nullptr) {
                            redisFree(connect); //释放连接
                        }
                        continue;
                    }
                    std::cout << "Connect redisServer success" << std::endl;
                    auto reply = (redisReply *) redisCommand(connect, "auth %s", _password.c_str()); //密码验证
                    if (reply->type == REDIS_REPLY_ERROR) {
                        std::cout << "Redis auth failure" << std::endl;
                        freeReplyObject(reply); //释放reply
                        continue;
                    }
                    freeReplyObject(reply); //释放reply
                    std::cout << "Redis auth success" << std::endl;
                    _mutex.lock();
                    _connects.push(connect);
                    _mutex.unlock();
                    if (_isStop == true) {
                        _isStop = false;
                    }
                }
                break;
            }
        });
        _allConnect_thread.detach();
    }
    if (_connects.empty()) {
        std::cout << "RedisConnectPool init failure" << std::endl;
        _isStop = true; //设置为true，防止调用_pool->getConnection()时阻塞
        return;
    }
    std::cout << "RedisConnectPool init success" << std::endl;
}

redisContext *RedisConnectPool::getConnection() {
    std::unique_lock<std::mutex> lock(_mutex);
    if (_isStop) {
        return nullptr;
    }
    _cond.wait(lock, [this]() {
        if (_isStop) {
            return true;
        }
        return !_connects.empty();
    });
    if (_isStop) {
        return nullptr;
    }
    auto connect = _connects.front();
    _connects.pop();
    return connect;
}

void RedisConnectPool::returnConnection(redisContext *connect) {
    std::lock_guard<std::mutex> lcok(_mutex);
    //检查连接是否断开,断开则释放连接
    if (connect->err == REDIS_ERR_IO || connect->err == REDIS_ERR_EOF) {
        redisFree(connect);
        reconnect();
        return;
    }
    if (_isStop) {
        return;
    }
    _connects.push(connect);
    _cond.notify_one();
}

RedisConnectPool::~RedisConnectPool() {
    close();
    std::cout << "RedisConnectPool is destructed " << std::endl;
}

void RedisConnectPool::close() {
    std::lock_guard<std::mutex> lock(_mutex);
    _isStop = true;
    _cond.notify_all();
    while (!_connects.empty()) {
        auto connect = _connects.front();
        _connects.pop();
        redisFree(connect);
    }
}

int RedisMgr::connectCount() {
    return _pool->_connects.size();
}

void RedisConnectPool::reconnect() {
    std::thread _reconnect_thread = std::thread([this]() {
        do {
            std::cout << "try to reconnect redisServer" << std::endl;
            redisContext *connect = redisConnect(_host.c_str(), _port);
            if (connect == nullptr || connect->err != 0) {
                if (connect != nullptr) {
                    redisFree(connect); //释放连接
                }
                std::this_thread::sleep_for(std::chrono::seconds(1)); // 添加延迟避免频繁重连
                continue;
            }
            std::cout << "Connect redisServer success" << std::endl;
            auto reply = (redisReply *) redisCommand(connect, "auth %s", _password.c_str()); //密码验证
            if (reply == nullptr || reply->type == REDIS_REPLY_ERROR) {
                if (reply != nullptr) {
                    std::cout << "Redis auth failure" << std::endl;
                    freeReplyObject(reply); //释放reply
                }
                redisFree(connect); // 释放连接
                std::this_thread::sleep_for(std::chrono::seconds(1)); // 添加延迟避免频繁重连
                continue;
            }
            freeReplyObject(reply); //释放reply
            std::cout << "Redis auth success" << std::endl;
            _mutex.lock();
            _connects.push(connect);
            _cond.notify_one();
            _mutex.unlock();
            break;
        } while (true);
    });
    _reconnect_thread.detach();
}
