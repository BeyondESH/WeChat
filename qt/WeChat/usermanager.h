#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QVector>
#include "singleton.h"

struct UserInfo
{
    int uid;
    QString name;
    QString email;
    QString avatar;
    QString status;
    bool isOnline;
};

struct GroupInfo
{
    int groupId;
    QString name;
    QString avatar;
    int ownerUid;
    int memberCount;
};

class UserManager : public Singleton<UserManager>
{
public:
    void setCurrentUser(int uid, const QString &name, const QString &email,
                       const QString &avatar, const QString &token);
    void clearCurrentUser();

    int getCurrentUid() const { return m_currentUid; }
    QString getCurrentUserName() const { return m_currentName; }
    QString getCurrentEmail() const { return m_currentEmail; }
    QString getCurrentAvatar() const { return m_currentAvatar; }
    QString getCurrentToken() const { return m_currentToken; }

    void addFriend(const UserInfo &friendInfo);
    void removeFriend(int uid);
    QVector<UserInfo> getFriendList() const { return m_friends; }
    void updateFriendList(const QVector<UserInfo> &friends);

    void addGroup(const GroupInfo &group);
    void removeGroup(int groupId);
    QVector<GroupInfo> getGroupList() const { return m_groups; }
    void updateGroupList(const QVector<GroupInfo> &groups);

private:
    int m_currentUid;
    QString m_currentName;
    QString m_currentEmail;
    QString m_currentAvatar;
    QString m_currentToken;

    QVector<UserInfo> m_friends;
    QVector<GroupInfo> m_groups;
};

#endif // USERMANAGER_H
