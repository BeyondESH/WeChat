//
// Created by Beyond on 2024/11/3.
//

#ifndef GATESERVER_CONST_H
#define GATESERVER_CONST_H
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <memory>
#include <iostream>
#include <functional>
#include "singleton.hpp"
#include <map>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <hiredis/hiredis.h>
#include <cassert>
#include <thread>

/**
 * @enum ErrorCodes
 * @brief 应用程序全局使用的错误代码
 *
 * 定义所有服务间一致的标准错误代码
 */
enum ErrorCodes
{
    SUCCESS = 0,              // 操作成功
    ERR_NETWORK = 1000,       // 网络连接错误
    ERROR_JSON = 1001,        // JSON解析或序列化错误
    RPC_FAILED = 1002,        // RPC调用失败
    VerifyCodeExpired = 1003, // 验证码已过期
    VerifyCodeError = 1004,   // 无效的验证码
    UserNotExist = 1005,      // 用户不存在
    UserExist = 1006,         // 用户已存在（MySQL错误 -2）
    PasswordError = 1007,     // 密码不正确
    EmailExist = 1008,        // 邮箱地址已注册（MySQL错误 -3）
    EmailNotExist = 1009,     // 邮箱地址未找到
    PasswordUpFailed = 1010,  // 密码更新失败
    ERR_MYSQL = 1011,         // MySQL数据库连接错误
    PasswordSame = 1012,      // 新密码与旧密码相同
    Token_Invalid = 1013,     // 无效或已过期的认证令牌
};

/**
 * @enum MSG_IDS
 * @brief 客户端-服务器通信的消息类型标识符
 *
 * 定义消息ID以正确路由协议消息
 */
enum MSG_IDS
{
    ID_CHAT_LOGIN = 1005,               // 聊天登录请求
    ID_CHAT_LOGIN_RSP = 1006,           // 聊天登录响应
    ID_SEARCH_USER_REQ = 1007,          // 搜索用户请求
    ID_SEARCH_USER_RSP = 1008,          // 搜索用户响应
    ID_ADD_FRIEND_REQ = 1009,           // 添加好友请求
    ID_ADD_FRIEND_RSP = 1010,           // 添加好友响应
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,    // 好友请求通知
    ID_AUTH_FRIEND_REQ = 1013,          // 好友请求授权
    ID_AUTH_FRIEND_RSP = 1014,          // 好友请求授权响应
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015,   // 好友授权通知
    ID_TEXT_CHAT_MSG_REQ = 1017,        // 文本消息请求
    ID_TEXT_CHAT_MSG_RSP = 1018,        // 文本消息响应
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, // 文本消息通知
};

// Redis键前缀和消息大小的常量
#define CODE_PREFIX "verifycode_" // Redis中验证码键的前缀
#define MSG_MAX_LEN 8192          // 最大消息长度（字节）
#define MSG_HEAD_SIZE 4           // 消息头大小（字节）
#define MSG_ID_SIZE 2             // 消息ID字段大小（字节）
#define QUEUE_MAX_SIZE 1000       // 消息处理队列的最大大小

#endif // GATESERVER_CONST_H
