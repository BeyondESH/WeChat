#ifndef USERMGR_H
#define USERMGR_H
/******************************************************************************
 *
 * @file       usermgr.h
 * @brief      XXXX Function
 *
 * @author     李佳承
 * @date       2025/03/12
 * @history
 *****************************************************************************/

#include "singleton.hpp"
class UserMgr:public Singleton<UserMgr>
{
    Q_OBJECT
    friend class Singleton<UserMgr>;
public:
    ~UserMgr();
    void clear();
    QString getUserName() const;
    void setUserName(const QString &newUserName);
    int getUid() const;
    void setUid(int newUid);
    QString getToken() const;
    void setToken(const QString &newToken);
    QString getPassword() const;
    void setPassword(const QString &newPassword);
    QString getAccount() const;
    void setAccount(const QString &newAccount);
private:
    UserMgr();
    UserMgr(const UserMgr&)=delete;
    UserMgr&operator=(const UserMgr&)=delete;
    QString _account;
    QString _userName;
    int _uid;
    QString _token;
    QString _password;
};

#endif // USERMGR_H
