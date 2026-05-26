#include "sessionmanager.h"

SessionManager::SessionManager()
    : m_nextSessionId(1)
{
}

void SessionManager::addSession(const ChatSession &session)
{
    ChatSession newSession = session;
    newSession.sessionId = m_nextSessionId++;
    m_sessions[newSession.sessionId] = newSession;
}

void SessionManager::removeSession(int sessionId)
{
    m_sessions.remove(sessionId);
}

void SessionManager::updateSession(int sessionId, const QString &lastMessage, const QString &time)
{
    if (m_sessions.contains(sessionId)) {
        m_sessions[sessionId].lastMessage = lastMessage;
        m_sessions[sessionId].lastTime = time;
    }
}

void SessionManager::incrementUnreadCount(int sessionId)
{
    if (m_sessions.contains(sessionId)) {
        m_sessions[sessionId].unreadCount++;
    }
}

void SessionManager::clearUnreadCount(int sessionId)
{
    if (m_sessions.contains(sessionId)) {
        m_sessions[sessionId].unreadCount = 0;
    }
}

QVector<ChatSession> SessionManager::getAllSessions() const
{
    return QVector<ChatSession>::fromList(m_sessions.values());
}

ChatSession* SessionManager::getSession(int sessionId)
{
    if (m_sessions.contains(sessionId)) {
        return &m_sessions[sessionId];
    }
    return nullptr;
}

ChatSession* SessionManager::getSessionByContact(int uid, bool isGroup)
{
    for (auto &session : m_sessions) {
        if (isGroup) {
            if (session.groupId == uid) {
                return &session;
            }
        } else {
            if (session.contactUid == uid) {
                return &session;
            }
        }
    }
    return nullptr;
}
