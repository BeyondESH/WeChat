//
// Created by Beyond on 2024/12/14.
//

#include "MySQLMgr.h"
#include "ConfigMgr.hpp"

MySQLConnectPool::MySQLConnectPool(const mysqlx::SessionSettings &settings, const int &poolSize): _settings(settings),
                                                                                                  _poolSize(poolSize), _isStop(false) {
    //创建连接
    for (int i = 0; i < _poolSize; ++i) {
        try {
            std::lock_guard<std::mutex> lock(_mutex);
            _sessions.emplace_back(std::make_shared<mysqlx::Session>(_settings));
        } catch (const mysqlx::Error &error) {
            std::cerr << "Failed to connect MysqlServer:" << error.what() << std::endl;
        }
    }
    _mutex.lock();
    int activeConnections=_sessions.size();
    _mutex.unlock();
    if (activeConnections < _poolSize) {
        reconnect();
    }

    if (_sessions.empty()) {
        std::cout << "Failed to connect MysqlServer,try to reconnect" << std::endl;
        _isStop = true;
        return;
    }
    std::cout << "MySQLConnectPool init success with " << _sessions.size() << " connections" << std::endl;
    checkConnection();
}

MySQLConnectPool::~MySQLConnectPool() {
    close();
    std::cout << "MySQLConnectPool is destructed" << std::endl;
}

std::shared_ptr<mysqlx::Session> MySQLConnectPool::getSession() {
    if (_isStop == true) {
        return nullptr;
    }
    try {
        std::unique_lock<std::mutex> lock(_mutex);
        _cond.wait(lock, [this]() {
            return _isStop || !_sessions.empty();
        });
        if (_sessions.empty()||_isStop==true) {
            return nullptr;
        }
        auto session = _sessions.back();
        _sessions.pop_back();
        return session;
    } catch (const mysqlx::Error &error) {
        std::cerr << "Failed to get MysqlServer connection:" << error.what() << std::endl;
        return nullptr;
    }
}

void MySQLConnectPool::returnSession(std::shared_ptr<mysqlx::Session> session) {
    if (_isStop==true) {
        return;
    }
    try {
        if (session!=nullptr) {
            session->sql("SELECT 1").execute();
            std::lock_guard<std::mutex> lock(_mutex);
            _sessions.emplace_back(session);
            _cond.notify_one();
        }else {
            reconnect();
        }
    } catch (const mysqlx::Error &error) {
        std::cerr << "Failed to return MysqlServer connection:" << error.what() << std::endl;
    }
}

void MySQLConnectPool::close() {
    _isStop=true;
    std::lock_guard<std::mutex> lock(_mutex);
    _cond.notify_all();
    for (auto &session:_sessions) {
        session->close();
    }
    _sessions.clear();
}


void MySQLConnectPool::reconnect(int num) {
    if (_isStop == true) {
        return;
    }
    std::thread([this,num]() {
        try {
            for (int i = num; i < _poolSize; ++i) {
                std::lock_guard<std::mutex> lock(_mutex);
                if (_isStop != true) {
                    _sessions.emplace_back(std::make_shared<mysqlx::Session>(_settings));
                    _cond.notify_one();
                }
            }
        } catch (const mysqlx::Error &error) {
            std::cerr << "Failed to reconnect MysqlServer:" << error.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }).detach();
}


void MySQLConnectPool::reconnect() {
    if (_isStop == true) {
        return;
    }
    std::thread([this]() {
        try {
            int num = _sessions.size();
            for (int i = num; i < _poolSize; ++i) {
                std::lock_guard<std::mutex> lock(_mutex);
                if (_isStop != true) {
                    _sessions.emplace_back(std::make_shared<mysqlx::Session>(_settings));
                    _cond.notify_one();
                }
            }
        } catch (const mysqlx::Error &error) {
            std::cerr << "Failed to reconnect MysqlServer:" << error.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }).detach();
}

//检查连接(死锁处理后)
void MySQLConnectPool::checkConnection() {
    if (_isStop == true) {
        return;
    }
    std::thread([this]() {
        while (1) {
            std::vector<std::shared_ptr<mysqlx::Session>> sessionsToReconnect;
            {
                std::lock_guard<std::mutex> lock(_mutex);
                auto session = _sessions.begin();
                while (session != _sessions.end()) {
                    try {
                        (*session)->sql("SELECT 1").execute();
                        ++session;
                    } catch (const mysqlx::Error &error) {
                        std::cerr << "Failed to check MysqlServer:" << error.what() << std::endl;
                        sessionsToReconnect.push_back(*session);
                        session = _sessions.erase(session);
                    }
                }
            }
            // 在锁外调用reconnect
            for (auto& session : sessionsToReconnect) {
                reconnect();
            }
            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
    }).detach();
}
//存在死锁问题:
// void MySQLConnectPool::checkConnection() {
//     if (_isStop == true) {
//         return;
//     }
//     std::thread([this]() {
//         while (1) {
//             std::lock_guard<std::mutex> lock(_mutex);
//             auto session = _sessions.begin();
//             while (session != _sessions.end()) {
//                 try {
//                     (*session)->sql("SELECT 1").execute();
//                     ++session;
//                 } catch (const mysqlx::Error &error) {
//                     std::cerr << "Failed to check MysqlServer:" << error.what() << std::endl;
//                     reconnect(); // 在持有锁的情况下调用reconnect
//                     session = _sessions.erase(session);
//                 }
//             }
//             std::this_thread::sleep_for(std::chrono::seconds(60));
//         }
//     }).detach();
// }


MySQLMgr::~MySQLMgr() {
    close();
    std::cout << "MySQLMgr is destructed" << std::endl;
}

bool MySQLMgr::queryUser(const std::string &user) {
    try {
        std::cout<<"Starting queryUser..."<<std::endl;
        auto session=_pool->getSession();
        std::cout<<"Got database session"<<std::endl;
        if (session==nullptr) {
            std::cout<<"Failed to get database session"<<std::endl;
            return false;
        }
        auto result=session->sql("SELECT `name` from wechat.user where `name`=\'"+user+"\'").execute();
        std::cout<<"Getting query result..."<<std::endl;
        if (result.count()==0) {
            std::cout<<"No result returned from query"<<std::endl;
            return false;
        }
        _pool->returnSession(session);
        return true;    //返回查询结果
    } catch (const mysqlx::Error &error) {
        std::cerr << "Failed to execute command:" << error.what() << std::endl;
        return false;
    }
}

bool MySQLMgr::queryEmail(const std::string &email) {
    try {
        std::cout<<"Starting queryEmail..."<<std::endl;
        auto session=_pool->getSession();
        if (session==nullptr) {
            std::cout<<"Failed to get database session"<<std::endl;
            return false;
        }
        auto result=session->sql("SELECT `email` from wechat.user where `email`=\'"+email+"\'").execute();  //查询邮箱是否存在
        std::cout<<"Getting query result..."<<std::endl;
        if (result.count()==0) {
            std::cout<<"No result returned from query"<<std::endl;
            return false;
        }
        _pool->returnSession(session);
        return true;    //返回查询结果
    } catch (const mysqlx::Error &error) {
        std::cerr<<"Failed to execute command:"<<error.what()<<std::endl;
        return false;
    }
    return true;
}

int MySQLMgr::regUser(const std::string &name, const std::string &password, const std::string &email) {
    std::cout << "Starting regUser..." << std::endl;
    auto session=_pool->getSession();
    std::cout << "Got database session" << std::endl;
    if (session==nullptr) {
        std::cout << "Failed to get database session" << std::endl;
        return -1;
    }
    try {
        std::cout << "Executing reg_user procedure..." << std::endl;
        auto result = session->sql("CALL reg_user(?,?,?,@result)").bind(name).bind(email).bind(password).execute();
        std::cout << "Getting procedure result..." << std::endl;
        auto output =session->sql("SELECT @result").execute();
        auto row=output.fetchOne();
        _pool->returnSession(session);
        if (!row) {
            std::cout << "No result returned from procedure" << std::endl;
            return -1;
        }
        int ret = row[0].get<int>();
        std::cout << "RegUser completed with result: " << ret << std::endl;
        return ret;//返回注册结果
    } catch (const mysqlx::Error &error) {
        std::cerr << "Failed to execute command:" << error.what() << std::endl;
        _pool->returnSession(session);
        return -1;
    }
}


void MySQLMgr::close() {
    _pool->close();
    std::cout<<"MySQLMgr is closed"<<std::endl;
}

MySQLMgr::MySQLMgr() {
    auto configMrg = ConfigMgr::getInstance();
    auto _settings= mysqlx::SessionSettings(configMrg["Mysql"]["host"], stoi(configMrg["Mysql"]["port"]),configMrg["Mysql"]["user"], configMrg["Mysql"]["password"], configMrg["Mysql"]["schema"]);
    _pool=std::make_unique<MySQLConnectPool>(_settings,std::thread::hardware_concurrency());
}
