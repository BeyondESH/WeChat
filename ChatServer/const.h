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

enum MSG_IDS {
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


#define CODE_PREFIX "verifycode_"
#define MSG_MAX_LEN 8192
#define MSG_HEAD_SIZE 4
#define MSG_ID_SIZE 2
#define QUEUE_MAX_SIZE 1000
#endif //GATESERVER_CONST_H
