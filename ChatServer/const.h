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
    ID_NOTIFY_AUTH_FRIEND_RSP = 1015,   // 好友授权通知
    ID_TEXT_CHAT_MSG_REQ = 1017,        // 文本消息请求
    ID_TEXT_CHAT_MSG_RSP = 1018,        // 文本消息响应
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, // 文本消息通知
    ID_IMAGE_CHAT_MSG_REQ = 1028,       // 图片消息请求
    ID_IMAGE_CHAT_MSG_RSP = 1029,       // 图片消息响应
    ID_NOTIFY_IMAGE_CHAT_MSG_REQ = 1030,// 图片消息通知
    ID_HEARTBEAT_PING = 1020,           // 客户端心跳请求
    ID_HEARTBEAT_PONG = 1021,           // 服务器心跳响应
    ID_FRIEND_LIST_REQ = 1022,          // 获取好友列表请求
    ID_FRIEND_LIST_RSP = 1023,          // 获取好友列表响应
    ID_GET_CHAT_HISTORY_REQ = 1024,     // 获取聊天历史请求
    ID_GET_CHAT_HISTORY_RSP = 1025,     // 获取聊天历史响应
    ID_RECENT_MESSAGES_RSP = 1026,      // 最近消息响应
    ID_FRIEND_REQUEST_LIST_RSP = 1027,  // 好友请求列表响应
    
    // 群聊相关消息ID
    ID_CREATE_GROUP_REQ = 2001,         // 创建群组请求
    ID_CREATE_GROUP_RSP = 2002,         // 创建群组响应
    ID_ADD_GROUP_MEMBER_REQ = 2003,     // 添加群成员请求
    ID_ADD_GROUP_MEMBER_RSP = 2004,     // 添加群成员响应
    ID_REMOVE_GROUP_MEMBER_REQ = 2005,  // 移除群成员请求
    ID_REMOVE_GROUP_MEMBER_RSP = 2006,  // 移除群成员响应
    ID_GROUP_CHAT_MSG_REQ = 2007,       // 群消息请求
    ID_GROUP_CHAT_MSG_RSP = 2008,       // 群消息响应
    ID_NOTIFY_GROUP_CHAT_MSG_REQ = 2009,// 群消息通知
    ID_GROUP_IMAGE_CHAT_MSG_REQ = 2016, // 群图片消息请求
    ID_GROUP_IMAGE_CHAT_MSG_RSP = 2017, // 群图片消息响应
    ID_NOTIFY_GROUP_IMAGE_CHAT_MSG_REQ = 2018,// 群图片消息通知
    ID_GET_GROUP_LIST_REQ = 2010,       // 获取群列表请求
    ID_GET_GROUP_LIST_RSP = 2011,       // 获取群列表响应
    ID_GET_GROUP_MEMBERS_REQ = 2012,    // 获取群成员请求
    ID_GET_GROUP_MEMBERS_RSP = 2013,    // 获取群成员响应
    ID_GET_GROUP_CHAT_HISTORY_REQ = 2014,// 获取群聊天历史请求
    ID_GET_GROUP_CHAT_HISTORY_RSP = 2015,// 获取群聊天历史响应
};

// Redis键前缀和消息大小的常量
#define CODE_PREFIX "verifycode_" // Redis中验证码键的前缀
#define MSG_MAX_LEN 8192          // 最大消息长度（字节）
#define MSG_HEAD_SIZE 4           // 消息头大小（字节）
#define MSG_ID_SIZE 2             // 消息ID字段大小（字节）
#define QUEUE_MAX_SIZE 1000       // 消息处理队列的最大大小

#endif // GATESERVER_CONST_H
