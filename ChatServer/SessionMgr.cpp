#include "SessionMgr.h"

SessionMgr::~SessionMgr() {
    std::lock_guard<std::mutex> lock(_mutex);
    _sessions.clear();
}

void SessionMgr::addSession(int uid, std::shared_ptr<CSession> session) {
    std::lock_guard<std::mutex> lock(_mutex);
    _sessions[uid] = session;
    std::cout << "添加用户会话，UID: " << uid << "，当前在线人数: " << _sessions.size() << std::endl;
}

void SessionMgr::removeSession(int uid) {
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _sessions.find(uid);
    if (it != _sessions.end()) {
        _sessions.erase(it);
        std::cout << "移除用户会话，UID: " << uid << "，当前在线人数: " << _sessions.size() << std::endl;
    }
}

std::shared_ptr<CSession> SessionMgr::getSessionByUid(int uid) {
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _sessions.find(uid);
    if (it != _sessions.end()) {
        return it->second;
    }
    return nullptr;
}

bool SessionMgr::isUserOnline(int uid) {
    std::lock_guard<std::mutex> lock(_mutex);
    return _sessions.find(uid) != _sessions.end();
}

size_t SessionMgr::getOnlineUserCount() {
    std::lock_guard<std::mutex> lock(_mutex);
    return _sessions.size();
}

void SessionMgr::broadcastMessage(const std::string& message, short msgId) {
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& pair : _sessions) {
        pair.second->send(message.c_str(), message.length(), msgId);
    }
}

std::vector<int> SessionMgr::getAllOnlineUsers() {
    std::vector<int> onlineUsers;
    std::lock_guard<std::mutex> lock(_mutex);
    
    for (auto& pair : _sessions) {
        onlineUsers.push_back(pair.first);
    }
    
    return onlineUsers;
}
