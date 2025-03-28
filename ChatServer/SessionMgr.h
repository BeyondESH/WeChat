#ifndef CHATSERVER_SESSIONMGR_H
#define CHATSERVER_SESSIONMGR_H

#include "singleton.hpp"
#include "CSession.h"
#include <map>
#include <mutex>

class SessionMgr : public Singleton<SessionMgr> {
    friend class Singleton<SessionMgr>;

public:
    ~SessionMgr();
    
    // 添加会话
    void addSession(int uid, std::shared_ptr<CSession> session);
    
    // 移除会话
    void removeSession(int uid);
    
    // 根据UID获取会话
    std::shared_ptr<CSession> getSessionByUid(int uid);
    
    // 检查用户是否在线
    bool isUserOnline(int uid);
    
    // 获取在线用户数量
    size_t getOnlineUserCount();
    
    // 广播消息给所有用户
    void broadcastMessage(const std::string& message, short msgId);
    
    // 获取所有在线用户ID
    std::vector<int> getAllOnlineUsers();

private:
    SessionMgr() {}
    std::map<int, std::shared_ptr<CSession>> _sessions;
    std::mutex _mutex;
};

#endif // CHATSERVER_SESSIONMGR_H
