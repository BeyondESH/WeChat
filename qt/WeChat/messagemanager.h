#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <QString>
#include <QVector>
#include <QMap>
#include "singleton.h"

struct MessageInfo
{
    int messageId;
    int fromUid;
    int toUid;
    int groupId;
    QString content;
    QString sendTime;
    int msgType;
    int status;
    QString imageType;
    int imageWidth;
    int imageHeight;
    int fileSize;
};

class MessageManager : public Singleton<MessageManager>
{
public:
    void addMessage(int chatId, bool isGroup, const MessageInfo &message);
    QVector<MessageInfo> getMessages(int chatId, bool isGroup) const;
    void clearMessages(int chatId, bool isGroup);

    void setLastReadTime(int chatId, bool isGroup, const QString &time);
    QString getLastReadTime(int chatId, bool isGroup) const;

private:
    QMap<QString, QVector<MessageInfo>> m_messages;
    QMap<QString, QString> m_lastReadTimes;
};

#endif // MESSAGEMANAGER_H
