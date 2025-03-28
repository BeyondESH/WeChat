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

SessionGuard MySQLConnectPool::getSessionGuard()
{
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
        // std::string cryptoPwd = Crypto::stringToSha256(password);
        // password已经在qt客户端被加密过了
        auto oldPwdResult = session->sql("SELECT `password` from wechat.user where `email`=\'" + email + "\'").execute();
        auto row = oldPwdResult.fetchOne();
        std::string oldPassword = row[0].get<std::string>();
        if (password == oldPassword)
        {
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

int MySQLMgr::loginAcconut(const std::string &user, const std::string &password)
{
    try
    {
        auto session = _pool->getSessionGuard();
        if (!session)
        {
            return -1;
        }
        // 查询用户是否存在
        auto existAccount = MySQLMgr::getInstance()->queryUser(user);
        if (existAccount == false)
        {
            std::cerr << "User is nonexistent" << std::endl;
            return -2;
        }
        // 查询密码是否正确
        auto result = session->sql("SELECT `password` from wechat.user where `name`=?").bind(user).execute();
        auto row = result.fetchOne();
        std::string reallyPassword = row[0].get<std::string>();
        if (password != reallyPassword)
        {
            std::cerr << "Password is error" << std::endl;
            return -3;
        }
        else
        {
            return 0;
        }
    }
    catch (const mysqlx::Error &error)
    {
        std::cerr << "Failed to login by account:" << error.what() << std::endl;
        return -1;
    }
}

int MySQLMgr::loginEmail(const std::string &email, const std::string &password)
{
    try
    {
        auto session = _pool->getSessionGuard();
        if (!session)
        {
            return -1;
        }
        // 查询用户是否存在
        auto existEmail = MySQLMgr::getInstance()->queryEmail(email);
        if (existEmail == false)
        {
            std::cerr << "User is nonexistent" << std::endl;
            return -2;
        }
        // 查询密码是否正确
        auto result = session->sql("SELECT `password` from wechat.user where `email`=?").bind(email).execute();
        auto row = result.fetchOne();
        std::string reallyPassword = row[0].get<std::string>();
        if (password != reallyPassword)
        {
            std::cerr << "Password is error" << std::endl;
            return -3;
        }
        else
        {
            return 0;
        }
    }
    catch (const mysqlx::Error &error)
    {
        std::cerr << "Failed to login by email:" << error.what() << std::endl;
        return -1;
    }
}

int MySQLMgr::getUID(const int mod, const std::string &str)
{
    try
    {
        auto session = _pool->getSessionGuard();
        if (!session)
        {
            return -1;
        }
        if (mod == 0)
        {
            auto result = session->sql("SELECT `id` from wechat.user where `email`=?").bind(str).execute();
            auto row = result.fetchOne();
            if (!row)
            {
                return -2;
            }
            else
            {
                return row[0].get<int>();
            }
        }
        else
        {
            auto result = session->sql("SELECT `id` from wechat.user where `name`=?").bind(str).execute();
            auto row = result.fetchOne();
            if (!row)
            {
                return -2;
            }
            else
            {
                return row[0].get<int>();
            }
        }
    }
    catch (const mysqlx::Error &error)
    {
        std::cerr << "Failed to get UID:" << error.what() << std::endl;
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

SessionGuard::SessionGuard(std::shared_ptr<mysqlx::Session> session) : _session(session)
{
}

SessionGuard::~SessionGuard()
{
    try
    {
        if (_session)
        {
            auto mgr = MySQLMgr::getInstance();
            if (mgr && mgr->_pool)
            {
                mgr->_pool->returnSession(_session);
            }
        }
    }
    catch (const std::bad_weak_ptr &)
    {
        // 单例已被销毁，忽略异常
        std::cerr << "Warning: MySQLMgr singleton was already destroyed when returning session" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error when returning session: " << e.what() << std::endl;
    }
}

SessionGuard::operator bool() const
{
    return _session != nullptr;
}

std::shared_ptr<mysqlx::Session> SessionGuard::get() const
{
    return _session;
}

std::shared_ptr<mysqlx::Session> SessionGuard::operator->() const
{
    return _session;
}

// 辅助函数：获取当前时间字符串
std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::vector<UserInfo> MySQLMgr::searchUserByName(const std::string &name) {
    std::vector<UserInfo> results;
    try {
        auto session = _pool->getSessionGuard();
        if (!session) {
            std::cerr << "Failed to get MySQL session" << std::endl;
            return results;
        }
        
        auto searchLike = "%" + name + "%";
        auto result = session->sql("SELECT uid, name, email, avatar, status FROM users WHERE name LIKE ?")
                             .bind(searchLike)
                             .execute();
        
        for (auto row : result.fetchAll()) {
            UserInfo user;
            user.uid = row[0].get<int>();
            user.name = row[1].get<std::string>();
            user.email = row[2].get<std::string>();
            user.avatar = row[3].isNull() ? "" : row[3].get<std::string>();
            user.status = row[4].isNull() ? "" : row[4].get<std::string>();
            results.push_back(user);
        }
    } catch (const std::exception &e) {
        std::cerr << "searchUserByName error: " << e.what() << std::endl;
    }
    return results;
}

UserInfo MySQLMgr::getUserById(int uid) {
    UserInfo user;
    try {
        auto session = _pool->getSessionGuard();
        if (!session) {
            std::cerr << "Failed to get MySQL session" << std::endl;
            return user;
        }
        
        auto result = session->sql("SELECT uid, name, email, avatar, status FROM users WHERE uid = ?")
                             .bind(uid)
                             .execute();
        
        auto row = result.fetchOne();
        if (row) {
            user.uid = row[0].get<int>();
            user.name = row[1].get<std::string>();
            user.email = row[2].get<std::string>();
            user.avatar = row[3].isNull() ? "" : row[3].get<std::string>();
            user.status = row[4].isNull() ? "" : row[4].get<std::string>();
        }
    } catch (const std::exception &e) {
        std::cerr << "getUserById error: " << e.what() << std::endl;
    }
    return user;
}

bool MySQLMgr::addFriendRequest(int fromUid, int toUid, const std::string &message) {
    try {
        auto session = _pool->getSessionGuard();
        if (!session) {
            std::cerr << "Failed to get MySQL session" << std::endl;
            return false;
        }
        
        // 先检查是否已经是好友
        auto checkResult = session->sql("SELECT 1 FROM friendships WHERE (user1_id = ? AND user2_id = ?) OR (user1_id = ? AND user2_id = ?)")
                                    .bind(fromUid).bind(toUid).bind(toUid).bind(fromUid)
                                    .execute();
        
        if (checkResult.count() > 0) {
            std::cerr << "Already friends" << std::endl;
            return false;
        }
        
        // 检查是否已有未处理的好友请求
        auto reqCheckResult = session->sql("SELECT 1 FROM friend_requests WHERE from_uid = ? AND to_uid = ? AND status = 'pending'")
                                     .bind(fromUid).bind(toUid)
                                     .execute();
        
        if (reqCheckResult.count() > 0) {
            std::cerr << "Friend request already sent" << std::endl;
            return false;
        }
        
        // 插入新的好友请求
        session->sql("INSERT INTO friend_requests (from_uid, to_uid, message, request_time, status) VALUES (?, ?, ?, NOW(), 'pending')")
               .bind(fromUid).bind(toUid).bind(message)
               .execute();
               
        return true;
    } catch (const std::exception &e) {
        std::cerr << "addFriendRequest error: " << e.what() << std::endl;
        return false;
    }
}

bool MySQLMgr::updateFriendRequestStatus(int requestId, bool accepted) {
    try {
        auto session = _pool->getSessionGuard();
        if (!session) {
            std::cerr << "Failed to get MySQL session" << std::endl;
            return false;
        }
        
        std::string status = accepted ? "accepted" : "rejected";
        session->sql("UPDATE friend_requests SET status = ?, response_time = NOW() WHERE id = ?")
               .bind(status).bind(requestId)
               .execute();
               
        return true;
    } catch (const std::exception &e) {
        std::cerr << "updateFriendRequestStatus error: " << e.what() << std::endl;
        return false;
    }
}

bool MySQLMgr::addFriendRelation(int uid1, int uid2) {
    try {
        auto session = _pool->getSessionGuard();
        if (!session) {
            std::cerr << "Failed to get MySQL session" << std::endl;
            return false;
        }
        
        // 确保低ID为user1_id，高ID为user2_id，保持一致性
        int user1 = std::min(uid1, uid2);
        int user2 = std::max(uid1, uid2);
        
        // 先检查是否已经是好友
        auto checkResult = session->sql("SELECT 1 FROM friendships WHERE user1_id = ? AND user2_id = ?")
                                    .bind(user1).bind(user2)
                                    .execute();
        
        if (checkResult.count() > 0) {
            return true; // 已经是好友
        }
        
        // 添加好友关系
        session->sql("INSERT INTO friendships (user1_id, user2_id, friendship_date) VALUES (?, ?, NOW())")
               .bind(user1).bind(user2)
               .execute();
               
        return true;
    } catch (const std::exception &e) {
        std::cerr << "addFriendRelation error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<UserInfo> MySQLMgr::getFriendList(int uid) {
    std::vector<UserInfo> friends;
    try {
        auto session = _pool->getSessionGuard();
        if (!session) {
            std::cerr << "Failed to get MySQL session" << std::endl;
            return friends;
        }
        
        // 查询所有好友
        auto result = session->sql("SELECT u.uid, u.name, u.email, u.avatar, u.status FROM users u "
                                 "JOIN friendships f ON (u.uid = f.user1_id OR u.uid = f.user2_id) "
                                 "WHERE (f.user1_id = ? OR f.user2_id = ?) AND u.uid != ?")
                             .bind(uid).bind(uid).bind(uid)
                             .execute();
        
        for (auto row : result.fetchAll()) {
            UserInfo user;
            user.uid = row[0].get<int>();
            user.name = row[1].get<std::string>();
            user.email = row[2].get<std::string>();
            user.avatar = row[3].isNull() ? "" : row[3].get<std::string>();
            user.status = row[4].isNull() ? "" : row[4].get<std::string>();
            friends.push_back(user);
        }
    } catch (const std::exception &e) {
        std::cerr << "getFriendList error: " << e.what() << std::endl;
    }
    return friends;
}

std::vector<std::pair<int, UserInfo>> MySQLMgr::getFriendRequests(int uid) {
    std::vector<std::pair<int, UserInfo>> requests;
    try {
        auto session = _pool->getSessionGuard();
        if (!session) {
            std::cerr << "Failed to get MySQL session" << std::endl;
            return requests;
        }
        
        // 查询所有待处理的好友请求
        auto result = session->sql("SELECT fr.id, u.uid, u.name, u.email, u.avatar, fr.message, fr.request_time "
                                 "FROM friend_requests fr "
                                 "JOIN users u ON fr.from_uid = u.uid "
                                 "WHERE fr.to_uid = ? AND fr.status = 'pending'")
                             .bind(uid)
                             .execute();
        
        for (auto row : result.fetchAll()) {
            int requestId = row[0].get<int>();
            UserInfo user;
            user.uid = row[1].get<int>();
            user.name = row[2].get<std::string>();
            user.email = row[3].get<std::string>();
            user.avatar = row[4].isNull() ? "" : row[4].get<std::string>();
            requests.push_back({requestId, user});
        }
    } catch (const std::exception &e) {
        std::cerr << "getFriendRequests error: " << e.what() << std::endl;
    }
    return requests;
}

int MySQLMgr::saveMessage(int fromUid, int toUid, const std::string &content, const std::string &time, int msgType) {
    int messageId = 0;
    try {
        auto session = _pool->getSessionGuard();
        if (!session) {
            std::cerr << "Failed to get MySQL session" << std::endl;
            return 0;
        }
        
        auto result = session->sql("INSERT INTO messages (from_uid, to_uid, content, send_time, msg_type, status) "
                                 "VALUES (?, ?, ?, ?, ?, 0)")
                             .bind(fromUid).bind(toUid).bind(content).bind(time).bind(msgType)
                             .execute();
                             
        messageId = result.getAutoIncrementValue();
    } catch (const std::exception &e) {
        std::cerr << "saveMessage error: " << e.what() << std::endl;
    }
    return messageId;
}

std::vector<std::map<std::string, std::string>> MySQLMgr::getChatHistory(int uid1, int uid2, int count, int offset) {
    std::vector<std::map<std::string, std::string>> messages;
    try {
        auto session = _pool->getSessionGuard();
        if (!session) {
            std::cerr << "Failed to get MySQL session" << std::endl;
            return messages;
        }
        
        // 获取两个用户之间的聊天记录
        auto result = session->sql("SELECT id, from_uid, to_uid, content, send_time, msg_type, status "
                                 "FROM messages "
                                 "WHERE (from_uid = ? AND to_uid = ?) OR (from_uid = ? AND to_uid = ?) "
                                 "ORDER BY send_time DESC LIMIT ? OFFSET ?")
                             .bind(uid1).bind(uid2).bind(uid2).bind(uid1)
                             .bind(count).bind(offset)
                             .execute();
        
        for (auto row : result.fetchAll()) {
            std::map<std::string, std::string> message;
            message["id"] = std::to_string(row[0].get<int>());
            message["from_uid"] = std::to_string(row[1].get<int>());
            message["to_uid"] = std::to_string(row[2].get<int>());
            message["content"] = row[3].get<std::string>();
            message["send_time"] = row[4].get<std::string>();
            message["msg_type"] = std::to_string(row[5].get<int>());
            message["status"] = std::to_string(row[6].get<int>());
            messages.push_back(message);
        }
        
        // 反转消息列表，使其按时间正序
        std::reverse(messages.begin(), messages.end());
    } catch (const std::exception &e) {
        std::cerr << "getChatHistory error: " << e.what() << std::endl;
    }
    return messages;
}

std::vector<std::map<std::string, std::string>> MySQLMgr::getLastMessages(int uid, int count) {
    std::vector<std::map<std::string, std::string>> messages;
    try {
        auto session = _pool->getSessionGuard();
        if (!session) {
            std::cerr << "Failed to get MySQL session" << std::endl;
            return messages;
        }
        
        // 查询获取每个对话的最后一条消息
        auto result = session->sql(
            "SELECT m.* FROM messages m "
            "INNER JOIN ( "
            "    SELECT "
            "        CASE "
            "            WHEN from_uid = ? THEN to_uid "
            "            WHEN to_uid = ? THEN from_uid "
            "        END AS contact_id, "
            "        MAX(id) as max_id "
            "    FROM messages "
            "    WHERE from_uid = ? OR to_uid = ? "
            "    GROUP BY contact_id "
            ") as latest ON m.id = latest.max_id "
            "ORDER BY m.send_time DESC "
            "LIMIT ?")
            .bind(uid).bind(uid).bind(uid).bind(uid).bind(count)
            .execute();
        
        for (auto row : result.fetchAll()) {
            std::map<std::string, std::string> message;
            message["id"] = std::to_string(row[0].get<int>());
            message["from_uid"] = std::to_string(row[1].get<int>());
            message["to_uid"] = std::to_string(row[2].get<int>());
            message["content"] = row[3].get<std::string>();
            message["send_time"] = row[4].get<std::string>();
            message["msg_type"] = std::to_string(row[5].get<int>());
            message["status"] = std::to_string(row[6].get<int>());
            
            // 确定对话对象ID
            int contactId = (std::stoi(message["from_uid"]) == uid) ? 
                           std::stoi(message["to_uid"]) : std::stoi(message["from_uid"]);
            message["contact_id"] = std::to_string(contactId);
            
            messages.push_back(message);
        }
    } catch (const std::exception &e) {
        std::cerr << "getLastMessages error: " << e.what() << std::endl;
    }
    return messages;
}
