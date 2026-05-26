#include "messagemanager.h"

QString makeKey(int chatId, bool isGroup)
{
    return QString("%1_%2").arg(isGroup ? "G" : "U").arg(chatId);
}

void MessageManager::addMessage(int chatId, bool isGroup, const MessageInfo &message)
{
    QString key = makeKey(chatId, isGroup);
    m_messages[key].append(message);
}

QVector<MessageInfo> MessageManager::getMessages(int chatId, bool isGroup) const
{
    QString key = makeKey(chatId, isGroup);
    return m_messages.value(key);
}

void MessageManager::clearMessages(int chatId, bool isGroup)
{
    QString key = makeKey(chatId, isGroup);
    m_messages.remove(key);
}

void MessageManager::setLastReadTime(int chatId, bool isGroup, const QString &time)
{
    QString key = makeKey(chatId, isGroup);
    m_lastReadTimes[key] = time;
}

QString MessageManager::getLastReadTime(int chatId, bool isGroup) const
{
    QString key = makeKey(chatId, isGroup);
    return m_lastReadTimes.value(key);
}
