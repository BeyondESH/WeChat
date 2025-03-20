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
#include <QByteArray>
#include <QNetworkReply>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QDir>
#include <QSettings>
#include <QCryptographicHash>
/**
 * @brief repolish 用来刷新qss
 */
extern std::function<void(QWidget*)> repolish;
extern std::function<QString(QString str)> stringToSha256;

//请求类型id
enum ReqId{
    ID_GET_VARIFY_CODE=1001,//获取验证码
    ID_REG_USER=1002,//注册用户
    ID_RESET_PASSWORD=1003,//重置密码
    ID_LOGIN=1004,//登录
    ID_CHAT_LOGIN=1005,//聊天登录
    ID_CHAT_LOGIN_RSP=1006,//聊天登录回包
    ID_SEARCH_USER_REQ = 1007, //
    ID_SEARCH_USER_RSP = 1008, //
    ID_ADD_FRIEND_REQ = 1009,  //
    ID_ADD_FRIEND_RSP  = 1010, //
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //
    ID_AUTH_FRIEND_REQ = 1013, //
    ID_AUTH_FRIEND_RSP = 1014, //
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //
    ID_TEXT_CHAT_MSG_REQ = 1017, //
    ID_TEXT_CHAT_MSG_RSP = 1018, //
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019,
};

//功能模块id
enum Modules {
    REGISTERMOD=0,//注册模块
    LOGINMOD=1,//登录模块
    RESETMOD=2,//重置密码模块
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
    ERR_MYSQL=1011,//mysql连接错误
    PasswordSame=1012,//新旧密码相同
    Token_Invalid=1013,//token无效
};

struct ServerInfo{
    QString host;
    QString port;
    QString token;
    int uid;
};


//列表项类型
enum ListItemType{
    CHAT_USER_ITEM,//聊天用户
    CONTACT_USER_ITEM,//联系人
    SEARCH_USER_ITEM,//搜索用户
    ADD_FRIEND_ITEM,//添加好友
    INVALID_ITEM,//无效项
    GROUP_ITEM,//群组
    MSG_ITEM,//消息
};

//消息类型
enum MessageType {
    Receive,  // 收到的消息
    Send       // 发送的消息
};

//消息内容类型
enum MessageContentType {
    Text,      // 文本消息
    Image,     // 图片消息
    File,      // 文件消息
    Voice,     // 语音消息
    Video      // 视频消息
};

//消息状态类型
enum MessageStatus {
    Sending,   // 发送中
    Sent,      // 已发送
    Delivered, // 已送达
    Read,      // 已读
    Failed     // 发送失败
};

extern QString gate_url_prefix;//网关url前缀

#endif // GLOBAL_H
