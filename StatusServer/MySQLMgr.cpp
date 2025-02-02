//
// Created by Beyond on 2024/12/14.
//

#include "MySQLMgr.h"
#include "ConfigMgr.hpp"
#include "Crypto.h"

MySQLConnectPool::MySQLConnectPool(const mysqlx::SessionSettings &settings, const int &poolSize) : _settings(settings),
                                                                                                   _poolSize(poolSize), _isStop(false)
{
    // 创建连接
    for (int i = 0; i < _poolSize; ++i)
    {
        try
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _sessions.emplace_back(std::make_shared<mysqlx::Session>(_settings));
        }
        catch (const mysqlx::Error &error)
        {
            std::cerr << "Failed to connect MysqlServer:" << error.what() << std::endl;
        }
    }
    _mutex.lock();
    int activeConnections = _sessions.size();
    _mutex.unlock();
    if (activeConnections < _poolSize)
    {
        reconnect();
    }

    if (_sessions.empty())
    {
        std::cout << "Failed to connect MysqlServer,try to reconnect" << std::endl;
        _isStop = true;
        return;
    }
    std::cout << "MySQLConnectPool init success with " << _sessions.size() << " connections" << std::endl;
    checkConnection();
}

MySQLConnectPool::~MySQLConnectPool()
{
    close();
    std::cout << "MySQLConnectPool is destructed" << std::endl;
}

std::shared_ptr<mysqlx::Session> MySQLConnectPool::getSession()
{
    if (_isStop == true)
    {
        return nullptr;
    }
    try
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _cond.wait(lock, [this]()
                   { return _isStop || !_sessions.empty(); });
        if (_sessions.empty() || _isStop == true)
        {
            return nullptr;
        }
        auto session = _sessions.back();
        _sessions.pop_back();
        return session;
    }
    catch (const mysqlx::Error &error)
    {
        std::cerr << "Failed to get MysqlServer connection:" << error.what() << std::endl;
        return nullptr;
    }
}

SessionGuard MySQLConnectPool::getSessionGuard() {
    return SessionGuard(getSession());
}

void MySQLConnectPool::returnSession(std::shared_ptr<mysqlx::Session> session)
{
    if (_isStop == true)
    {
        return;
    }
    try
    {
        if (session != nullptr)
        {
            session->sql("SELECT 1").execute();
            std::lock_guard<std::mutex> lock(_mutex);
            _sessions.emplace_back(session);
            _cond.notify_one();
        }
        else
        {
            reconnect();
        }
    }
    catch (const mysqlx::Error &error)
    {
        std::cerr << "Failed to return MysqlServer connection:" << error.what() << std::endl;
    }
}

void MySQLConnectPool::close()
{
    _isStop = true;
    std::lock_guard<std::mutex> lock(_mutex);
    _cond.notify_all();
    for (auto &session : _sessions)
    {
        session->close();
    }
    _sessions.clear();
}

void MySQLConnectPool::reconnect(int num)
{
    if (_isStop == true)
    {
        return;
    }
    std::thread([this, num]()
                {
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
        } })
        .detach();
}

void MySQLConnectPool::reconnect()
{
    if (_isStop == true)
    {
        return;
    }
    std::thread([this]()
                {
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
        } })
        .detach();
}

// 检查连接(死锁处理后)
void MySQLConnectPool::checkConnection()
{
    if (_isStop == true)
    {
        return;
    }
    std::thread([this]()
                {
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
        } })
        .detach();
}
// 存在死锁问题:
//  void MySQLConnectPool::checkConnection() {
//      if (_isStop == true) {
//          return;
//      }
//      std::thread([this]() {
//          while (1) {
//              std::lock_guard<std::mutex> lock(_mutex);
//              auto session = _sessions.begin();
//              while (session != _sessions.end()) {
//                  try {
//                      (*session)->sql("SELECT 1").execute();
//                      ++session;
//                  } catch (const mysqlx::Error &error) {
//                      std::cerr << "Failed to check MysqlServer:" << error.what() << std::endl;
//                      reconnect(); // 在持有锁的情况下调用reconnect
//                      session = _sessions.erase(session);
//                  }
//              }
//              std::this_thread::sleep_for(std::chrono::seconds(60));
//          }
//      }).detach();
//  }

MySQLMgr::~MySQLMgr()
{
    close();
    std::cout << "MySQLMgr is destructed" << std::endl;
}

bool MySQLMgr::queryUser(const std::string &user)
{
    try
    {
        std::cout << "Starting queryUser..." << std::endl;
        auto session = _pool->getSessionGuard();
        std::cout << "Got database session" << std::endl;
        if (!session)
        {
            std::cout << "Failed to get database session" << std::endl;
            return false;
        }
        auto result = session->sql("SELECT `name` from wechat.user where `name`=\'" + user + "\'").execute();
        std::cout << "Getting query result..." << std::endl;
        if (result.count() == 0)
        {
            std::cout << "No result returned from query" << std::endl;
            return false;
        }
        return true; // 返回查询结果
    }
    catch (const mysqlx::Error &error)
    {
        std::cerr << "Failed to execute command:" << error.what() << std::endl;
        return false;
    }
}

bool MySQLMgr::queryEmail(const std::string &email)
{
    try
    {
        std::cout << "Starting queryEmail..." << std::endl;
        auto session = _pool->getSessionGuard();
        if (!session)
        {
            std::cout << "Failed to get database session" << std::endl;
            return false;
        }
        auto result = session->sql("SELECT `email` from wechat.user where `email`=\'" + email + "\'").execute(); // 查询邮箱是否存在
        std::cout << "Getting query result..." << std::endl;
        if (result.count() == 0)
        {
            std::cout << "No result returned from query" << std::endl;
            return false;
        }
        return true; // 返回查询结果
    }
    catch (const mysqlx::Error &error)
    {
        std::cerr << "Failed to execute command:" << error.what() << std::endl;
        return false;
    }
    return true;
}

int MySQLMgr::regUser(const std::string &name, const std::string &password, const std::string &email)
{
    try
    {
        std::cout << "Starting regUser..." << std::endl;
        auto session = _pool->getSessionGuard();
        std::cout << "Got database session" << std::endl;
        if (!session)
        {
            std::cout << "Failed to get database session" << std::endl;
            return -1;
        }
        std::cout << "Executing reg_user procedure..." << std::endl;
        auto result = session->sql("CALL reg_user(?,?,?,@result)").bind(name).bind(email).bind(password).execute();
        std::cout << "Getting procedure result..." << std::endl;
        auto output = session->sql("SELECT @result").execute();
        auto row = output.fetchOne();
        if (!row)
        {
            std::cout << "No result returned from procedure" << std::endl;
            return -1;
        }
        int ret = row[0].get<int>();
        std::cout << "RegUser completed with result: " << ret << std::endl;
        return ret; // 返回注册结果
    }
    catch (const mysqlx::Error &error)
    {
        std::cerr << "Failed to execute command:" << error.what() << std::endl;
        return -1;
    }
}

// 重置密码 -1连接错误 -2邮箱不存在 -3密码相同
int MySQLMgr::resetPassword(const std::string &email, const std::string &password)
{
    try
    {
        std::cout << "Starting resetPassword..." << std::endl;
        auto session = _pool->getSessionGuard();
        if (!session)
        {
            std::cerr << "Failed to get database session" << std::endl;
            return -1;
        }
        // 查询邮箱是否存在
        bool queryResult = MySQLMgr::getInstance()->queryEmail(email);
        if (queryResult == false)
        {
            return -2;
        }
        // 查询密码是否相同
        //std::string cryptoPwd = Crypto::stringToSha256(password);
        //password已经在qt客户端被加密过了
        auto oldPwdResult = session->sql("SELECT `password` from wechat.user where `email`=\'" + email + "\'").execute();
        auto row = oldPwdResult.fetchOne();
        std::string oldPassword=row[0].get<std::string>();
        if(password==oldPassword){
            return -3;
        }
        // 更新密码
        auto result = session->sql("UPDATE wechat.user SET `password`=? where `email`=?").bind(password).bind(email).execute();
        return 0;
    }
    catch (const mysqlx::Error &error)
    {
        std::cerr << "Failed to execute command:" << error.what() << std::endl;
        return -1;
    }
}

int MySQLMgr::loginAcconut(const std::string &user, const std::string &password) {
    try {
        auto session=_pool->getSessionGuard();
        if (!session) {
            return -1;
        }
        //查询用户是否存在
        auto existAccount=MySQLMgr::getInstance()->queryUser(user);
        if (existAccount==false) {
            std::cerr<<"User is nonexistent"<<std::endl;
            return -2;
        }
        //查询密码是否正确
        auto result=session->sql("SELECT `password` from wechat.user where `name`=?").bind(user).execute();
        auto row=result.fetchOne();
        std::string reallyPassword=row[0].get<std::string>();
        if (password!=reallyPassword) {
            std::cerr<<"Password is error"<<std::endl;
            return -3;
        }else{
            return 0;
        }
    } catch (const mysqlx::Error &error) {
        std::cerr<<"Failed to login by account:"<<error.what()<<std::endl;
        return -1;
    }
}

int MySQLMgr::loginEmail(const std::string &email, const std::string &password) {
    try {
        auto session=_pool->getSessionGuard();
        if (!session) {
            return -1;
        }
        //查询用户是否存在
        auto existEmail=MySQLMgr::getInstance()->queryEmail(email);
        if (existEmail==false) {
            std::cerr<<"User is nonexistent"<<std::endl;
            return -2;
        }
        //查询密码是否正确
        auto result=session->sql("SELECT `password` from wechat.user where `email`=?").bind(email).execute();
        auto row=result.fetchOne();
        std::string reallyPassword=row[0].get<std::string>();
        if (password!=reallyPassword) {
            std::cerr<<"Password is error"<<std::endl;
            return -3;
        }else{
            return 0;
        }
    } catch (const mysqlx::Error &error) {
        std::cerr<<"Failed to login by email:"<<error.what()<<std::endl;
        return -1;
    }
}

void MySQLMgr::close()
{
    _pool->close();
    std::cout << "MySQLMgr is closed" << std::endl;
}

MySQLMgr::MySQLMgr()
{
    auto configMrg = ConfigMgr::getInstance();
    auto _settings = mysqlx::SessionSettings(configMrg["Mysql"]["host"], stoi(configMrg["Mysql"]["port"]), configMrg["Mysql"]["user"], configMrg["Mysql"]["password"], configMrg["Mysql"]["schema"]);
    _pool = std::make_unique<MySQLConnectPool>(_settings, std::thread::hardware_concurrency());
}

SessionGuard::SessionGuard(std::shared_ptr<mysqlx::Session> session):_session(session) {

}

SessionGuard::~SessionGuard() {
    if (_session) {
            MySQLMgr::getInstance()->_pool->returnSession(_session);
    }
}

SessionGuard::operator bool() const {
    return _session != nullptr;
}

std::shared_ptr<mysqlx::Session> SessionGuard::get() const {
    return _session;
}

std::shared_ptr<mysqlx::Session> SessionGuard::operator->() const {
    return _session;
}
