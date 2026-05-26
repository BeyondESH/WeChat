#include "usermanager.h"

void UserManager::setCurrentUser(int uid, const QString &name, const QString &email,
                                const QString &avatar, const QString &token)
{
    m_currentUid = uid;
    m_currentName = name;
    m_currentEmail = email;
    m_currentAvatar = avatar;
    m_currentToken = token;
}

void UserManager::clearCurrentUser()
{
    m_currentUid = 0;
    m_currentName.clear();
    m_currentEmail.clear();
    m_currentAvatar.clear();
    m_currentToken.clear();
    m_friends.clear();
    m_groups.clear();
}

void UserManager::addFriend(const UserInfo &friendInfo)
{
    m_friends.append(friendInfo);
}

void UserManager::removeFriend(int uid)
{
    for (int i = 0; i < m_friends.size(); ++i) {
        if (m_friends[i].uid == uid) {
            m_friends.remove(i);
            break;
        }
    }
}

void UserManager::updateFriendList(const QVector<UserInfo> &friends)
{
    m_friends = friends;
}

void UserManager::addGroup(const GroupInfo &group)
{
    m_groups.append(group);
}

void UserManager::removeGroup(int groupId)
{
    for (int i = 0; i < m_groups.size(); ++i) {
        if (m_groups[i].groupId == groupId) {
            m_groups.remove(i);
            break;
        }
    }
}

void UserManager::updateGroupList(const QVector<GroupInfo> &groups)
{
    m_groups = groups;
}
