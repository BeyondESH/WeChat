#include "usermgr.h"

UserMgr::~UserMgr()
{

}

void UserMgr::clear()
{
    _account.clear();
    _userName.clear();
    _uid=0;
    _token.clear();
    _password.clear();
}

QString UserMgr::getUserName() const
{
    return _userName;
}

void UserMgr::setUserName(const QString &newUserName)
{
    _userName = newUserName;
}

int UserMgr::getUid() const
{
    return _uid;
}

void UserMgr::setUid(int newUid)
{
    _uid = newUid;
}

QString UserMgr::getToken() const
{
    return _token;
}

void UserMgr::setToken(const QString &newToken)
{
    _token = newToken;
}

QString UserMgr::getPassword() const
{
    return _password;
}

void UserMgr::setPassword(const QString &newPassword)
{
    _password = newPassword;
}

UserMgr::UserMgr()
{

}

QString UserMgr::getAccount() const
{
    return _account;
}

void UserMgr::setAccount(const QString &newAccount)
{
    _account = newAccount;
}

QString UserMgr::getHeadPath() const
{
    return QString(":/img/loginDialog/img/avatar/me.jpg");
}
