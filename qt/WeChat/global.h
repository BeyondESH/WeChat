#ifndef GLOBAL_H
#define GLOBAL_H
/******************************************************************************
 *
 * @file       global.h
 * @brief      全局对象
 *
 * @author     李佳承
 * @date       2024/11/02
 * @history
 *****************************************************************************/
#include <QWidget>
#include <functional>
#include "QStyle"
#include <QRegularExpression>//正则表达式
#include <memory.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <QByteArray>
#include <QNetworkReply>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QDir>
#include <QSettings>
/**
 * @brief repolish 用来刷新qss
 */
extern std::function<void(QWidget*)> repolish;

//请求类型id
enum ReqId{
    ID_GET_VARIFY_CODE=1001,//获取验证码
    ID_REG_USER=1002,//注册用户
};

//功能模块id
enum Modules {
    REGISTERMOD=0,//注册模块
};

//错误码
enum ErrorCodes{
    SUCCESS=0,
    ERR_NETWORK=1000,//网络错误
    ERROR_JSON=1001,//json解析失败
    RPC_FAILED=1002,//rpc调用失败
    VerifyCodeExpired=1003,//验证码过期
    VerifyCodeError=1004,//验证码错误
    UserNotExist=1005,//用户不存在
    UserExist=1006,//用户已存在，mysql错误返回-2
    PasswordError=1007,//密码错误
    EmailExist=1008,//邮箱已存在，mysql错误返回-3
    EmailNotExist=1009,//邮箱不存在
    PasswordUpFailed=1010,//密码修改失败
};


extern QString gate_url_prefix;//网关url前缀

#endif // GLOBAL_H
