//
// Created by Beyond on 2024/12/14.
//

#ifndef MYSQLMGR_H
#define MYSQLMGR_H

#include <mysqlx/xdevapi.h>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include "singleton.hpp"

struct UserInfo {
    int uid = 0;
    std::string name;
    std::string email;
    std::string avatar;
    std::string status;
};

struct GroupInfo {
    int groupId = 0;
    std::string name;
    int ownerUid = 0;
    std::string avatar;
    std::string createdAt;
};

class SessionGuard; // 提前声明SessionGuard类

class MySQLConnectPool
{
public:
    MySQLConnectPool(const mysqlx::SessionSettings &settings, const int &poolSize);
    ~MySQLConnectPool();
    std::shared_ptr<mysqlx::Session> getSession();
    SessionGuard getSessionGuard();
    void returnSession(std::shared_ptr<mysqlx::Session> session);
    void close();

private:
    std::vector<std::shared_ptr<mysqlx::Session>> _sessions;
    std::mutex _mutex;
    std::condition_variable _cond;
    std::atomic_bool _isStop;
    int _poolSize;
    mysqlx::SessionSettings _settings;
    void reconnect(int num);
    void reconnect();
    void checkConnection();
};

class MySQLMgr : public Singleton<MySQLMgr>
{
    friend class Singleton<MySQLMgr>;
    friend class SessionGuard;

public:
    ~MySQLMgr();
    // 基本的数据库操作接口
    bool execute(const std::string &sql);
    bool query(const std::string &sql, std::vector<std::map<std::string, std::string>> &result);
    bool queryUser(const std::string &user); // true存在 false不存在
    bool queryEmail(const std::string &email);
    int regUser(const std::string &name, const std::string &password, const std::string &email);
    int resetPassword(const std::string &email, const std::string &password); // 重置密码 -1连接错误 -2邮箱不存在 -3其他错误
    int loginAcconut(const std::string &user, const std::string &password);   // 账号登录 -1连接错误 -2用户不存在 -3密码错误
    int loginEmail(const std::string &email, const std::string &password);    // 邮箱登录 -1连接错误 -2用户不存在 -3密码错误
    int getUID(const int mod,const std::string &str); // 获取用户ID -1连接错误 -2用户不存在, mod=0表示邮箱 mod=1表示用户名
    
    // 用户相关接口
    std::vector<UserInfo> searchUserByName(const std::string &name);
    UserInfo getUserById(int uid);
    
    // 好友相关接口
    std::vector<UserInfo> getFriendList(int uid);
    std::vector<std::pair<int, UserInfo>> getFriendRequests(int uid);
    bool addFriendRequest(int fromUid, int toUid, const std::string &message);
    bool updateFriendRequestStatus(int requestId, bool accepted);
    bool addFriendRelation(int uid1, int uid2);
    
    // 消息相关接口
    int saveMessage(int fromUid, int toUid, const std::string &content, const std::string &time, int msgType);
    std::vector<std::map<std::string, std::string>> getChatHistory(int uid1, int uid2, int count, int offset);
    std::vector<std::map<std::string, std::string>> getLastMessages(int uid, int count);
    
    // 群聊相关接口
    int createGroup(int ownerUid, const std::string &name, const std::string &avatar);
    bool addGroupMember(int groupId, int uid, const std::string &role = "member");
    bool removeGroupMember(int groupId, int uid);
    bool updateGroupMemberRole(int groupId, int uid, const std::string &role);
    std::vector<UserInfo> getGroupMembers(int groupId);
    std::vector<GroupInfo> getUserGroups(int uid);
    GroupInfo getGroupInfo(int groupId);
    bool updateGroupName(int groupId, const std::string &newName);
    bool updateGroupAvatar(int groupId, const std::string &avatar);
    int saveGroupMessage(int fromUid, int groupId, const std::string &content, const std::string &time, int msgType);
    std::vector<std::map<std::string, std::string>> getGroupChatHistory(int groupId, int count, int offset);
    
    void close();

private:
    MySQLMgr();
    MySQLMgr(const MySQLMgr &) = delete;
    MySQLMgr &operator=(const MySQLMgr &) = delete;
    std::vector<std::map<std::string, std::string>> _result; // 查询结果
    std::unique_ptr<MySQLConnectPool> _pool;
};

class SessionGuard
{
public:
    explicit SessionGuard(std::shared_ptr<mysqlx::Session> session);
    ~SessionGuard();
    // 禁用拷贝构造和赋值
    SessionGuard(const SessionGuard &) = delete;
    SessionGuard &operator=(const SessionGuard &) = delete;
    // 获取原始session指针
    std::shared_ptr<mysqlx::Session> get() const;
    std::shared_ptr<mysqlx::Session> operator->() const;
    operator bool() const;
private:
    std::shared_ptr<mysqlx::Session> _session;
};

#endif // MYSQLMGR_H
