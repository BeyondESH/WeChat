#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QString>
#include <QVector>
#include <QMap>
#include "singleton.h"

struct ChatSession
{
    int sessionId;
    int uid;
    int contactUid;
    int groupId;
    QString lastMessage;
    QString lastTime;
    int unreadCount;
    bool isGroup;
};

class SessionManager : public Singleton<SessionManager>
{
public:
    void addSession(const ChatSession &session);
    void removeSession(int sessionId);
    void updateSession(int sessionId, const QString &lastMessage, const QString &time);
    void incrementUnreadCount(int sessionId);
    void clearUnreadCount(int sessionId);
    QVector<ChatSession> getAllSessions() const;
    ChatSession* getSession(int sessionId);
    ChatSession* getSessionByContact(int uid, bool isGroup);

private:
    QMap<int, ChatSession> m_sessions;
    int m_nextSessionId;
};

#endif // SESSIONMANAGER_H
