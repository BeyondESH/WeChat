//
// Created by Beyond on 2024/12/14.
//

#ifndef MYSQLMGR_H
#define MYSQLMGR_H

#define STATIC_CONCPP
#define CONCPP_BUILD_STATIC
#include <mysqlx/xdevapi.h>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include "singleton.hpp"

class MySQLConnectPool {
public:
    MySQLConnectPool(const mysqlx::SessionSettings& settings,const int &poolSize);
    ~MySQLConnectPool();
    std::shared_ptr<mysqlx::Session> getSession();
    void releaseConnection(std::shared_ptr<mysqlx::Session> session);
    void close();
private:
    std::vector<std::shared_ptr<mysqlx::Session>> _sessions;
    std::mutex _mutex;
    std::condition_variable _cond;
    bool _isStop;
    int _poolSize;
    mysqlx::SessionSettings _settings;
    void reconnect();
};

class MySQLMgr : public Singleton<MySQLMgr> {
    friend class Singleton<MySQLMgr>;
public:
    ~MySQLMgr();
    // 基本的数据库操作接口
    bool execute(const std::string& sql);
    bool query(const std::string& sql, std::vector<std::map<std::string, std::string>>& result);
    void close();
private:
    MySQLMgr()=default;
    MySQLMgr(const MySQLMgr&) = delete;
    MySQLMgr& operator=(const MySQLMgr&) = delete;
    std::vector<std::map<std::string, std::string>> _result;//查询结果
    std::unique_ptr<MySQLConnectPool> _pool;
    mysqlx::SessionSettings _settings;
};

#endif //MYSQLMGR_H
