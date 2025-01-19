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
    SUCCESS=0,//成功
    ERR_NETWORK=1000,//网络错误
    ERROR_JSON=1001,//json解析失败
    RPC_FAILED=1002,//rpc调用失败
    VerifyCodeExpired=1003,//验证码过期
    VerifyCodeError=1004,//验证码错误
    UserNotExist=1005,//用户不存在
    UserExist=1006,//用户已存在
    PasswordError=1007,//密码错误
    EmailExist=1008,//邮箱已存在
    EmailNotExist=1009,//邮箱不存在
    PasswordUpFailed=1010,//密码修改失败
};

#define CODE_PREFIX "verifycode_"

#endif //GATESERVER_CONST_H
