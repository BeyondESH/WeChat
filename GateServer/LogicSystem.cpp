//
// Created by Beyond on 2024/11/10.
//
#include "VerifyGrpcClient.h"
#include "LogicSystem.h"
#include "HttpConnection.h"
#include "MySQLMgr.h"
#include "RedisMgr.h"
#include "Crypto.h"
using json = nlohmann::json;

bool LogicSystem::handleGet(std::string path, std::shared_ptr<HttpConnection> connection) {
    //未找到path
    if (_getHandlers.find(path) == _getHandlers.end()) {
        return false;
    }
    _getHandlers[path](connection);
    return true;
}

bool LogicSystem::handlePost(std::string path, std::shared_ptr<HttpConnection> connection) {
    //未找到path
    if (_postHandlers.find(path) == _postHandlers.end()) {
        return false;
    }
    _postHandlers[path](connection);
    return true;
}

LogicSystem::~LogicSystem() {
}

void LogicSystem::regGet(std::string url, HttpHandler handler) {
    _getHandlers.insert({url, handler});
}

LogicSystem::LogicSystem() {
    //注册get请求
    regGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        boost::beast::ostream(connection->_response.body()) << "receive get_test request";
    });

    //注册post请求
    regPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data()); //获取body
        std::cout << "receive body is:" << body_str << std::endl;
        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        json src_root = json::parse(body_str); //接收的json
        json root; //发送的json
        if (src_root.is_null()) {
            std::cout << "Failed to parse JSON data" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string json_str = root.dump(4);
            boost::beast::ostream(connection->_response.body()) << json_str;
            return true;
        }
        if (src_root.contains("email")) {
            const auto email = src_root["email"];
            std::cout << "email is:" << email << std::endl;
            root["email"] = email;
            auto codeReply = VerifyGrpcClient::getInstance()->GetVerifyCode(email); //调用grpc获取验证码
            root["error"] = codeReply.error(); //错误码
            root["code"] = codeReply.code(); //验证码
            std::string json_str = root.dump(4);
            boost::beast::ostream(connection->_response.body()) << json_str;
            return true;
        } else {
            std::cout << "email is inexistence" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string json_str = root.dump(4);
            boost::beast::ostream(connection->_response.body()) << json_str;
            return true;
        }
    });

    regPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data()); //获取body
        std::cout << "receive body is:" << body_str << std::endl;
        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        json src_root = json::parse(body_str); //接收的json
        json root; //发送的json
        if (src_root.is_null()) {
            std::cout << "Failed to parse JSON data" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string json_str = root.dump(4);
            std::cout << "sent json is:" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return true;
        }

        //查找并验证redis中的验证码
        const std::string email = src_root["email"];
        std::string verifyCode = src_root["verifyCode"];
        std::string value;
        bool result_get = RedisMgr::getInstance()->get(CODE_PREFIX + email, value);
        //验证码过期或不存在
        if (result_get == false) {
            std::cout << "verifycode is nonexistent" << std::endl;
            root["error"] = ErrorCodes::VerifyCodeExpired;
            auto json_str = root.dump(4);
            std::cout << "sent json is" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return true;
        }

        //验证码错误
        if (verifyCode != value) {
            std::cout << "verifycode is error" << std::endl;
            root["error"] = ErrorCodes::VerifyCodeError;
            auto json_str = root.dump(4);
            std::cout << "sent json is:" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return true;
        }
        std::cout << "verifycode is right" << std::endl;
        //查找MySQL数据库
        const std::string name = src_root["user"];
        const std::string password = src_root["password"];
        auto result = MySQLMgr::getInstance()->regUser(name, password, email);
        std::cout << "regUser result is:" << result << std::endl;
        switch (result) {
            case -1:
                root["error"] = ErrorCodes::ERR_NETWORK; //mysql连接错误
                break;
            case -2:
                root["error"] = ErrorCodes::UserExist; //用户已存在
                break;
            case -3:
                root["error"] = ErrorCodes::EmailExist; //邮箱已存在
                break;
            default:
                root["error"] = ErrorCodes::SUCCESS;
        }
        root["email"] = email;
        root["verifyCode"] = verifyCode;
        root["user"] = src_root["user"];
        root["password"] = src_root["password"];
        auto json_str = root.dump(4);
        std::cout << "sent json is:" << json_str << std::endl;
        boost::beast::ostream(connection->_response.body()) << json_str;
        return true;
    });

    regPost("/reset_password", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data()); //获取body
        std::cout << "receive body is:" << body_str << std::endl;
        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        json src_root = json::parse(body_str); //接收的json
        json root; //发送的json
        if (src_root.is_null()) {
            std::cout << "Failed to parse JSON data" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string json_str = root.dump(4);
            std::cout << "sent json is:" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return true;
        }

        //查找并验证redis中的验证码
        const std::string email = src_root["email"];
        std::string verifyCode = src_root["verifyCode"];
        std::string value;
        bool result_get = RedisMgr::getInstance()->get(CODE_PREFIX + email, value);
        //验证码过期或不存在
        if (result_get == false) {
            std::cout << "verifycode is nonexistent" << std::endl;
            root["error"] = ErrorCodes::VerifyCodeExpired;
            auto json_str = root.dump(4);
            std::cout << "sent json is" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return true;
        }

        //验证码错误
        if (verifyCode != value) {
            std::cout << "verifycode is error" << std::endl;
            root["error"] = ErrorCodes::VerifyCodeError;
            auto json_str = root.dump(4);
            std::cout << "sent json is:" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return true;
        }
        std::cout << "verifycode is right" << std::endl;
        //查找MySQL数据库
        const std::string password = src_root["password"];
        auto result = MySQLMgr::getInstance()->resetPassword(email, password);
        std::cout << "resetPassword result is:" << result << std::endl;
        switch (result) {
            case -1:
                root["error"] = ErrorCodes::ERR_NETWORK; //mysql连接错误
                break;
            case -2:
                root["error"] = ErrorCodes::EmailNotExist; //邮箱不存在
                break;
            case -3:
                root["error"] = ErrorCodes::PasswordSame; //新旧密码相同
                break;
            default:
                root["error"] = ErrorCodes::SUCCESS;
        }
        root["email"] = email;
        root["verifyCode"] = verifyCode;
        root["password"] = src_root["password"];
        auto json_str = root.dump(4);
        std::cout << "sent json is:" << json_str << std::endl;
        boost::beast::ostream(connection->_response.body()) << json_str;
        return true;
    });

    //账户登录
    regPost("/account_login", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data()); //获取body
        std::cout << "receive body is:" << body_str << std::endl;
        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        json src_root = json::parse(body_str); //接收的json
        json root; //发送的json
        if (src_root.is_null()) {
            std::cout << "Failed to parse JSON data" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string json_str = root.dump(4);
            std::cout << "sent json is:" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return true;
        }

        std::string user = src_root["user"];
        std::string password = src_root["password"];
        int result = MySQLMgr::getInstance()->loginAcconut(user, password);
        switch (result) {
            case -1:
                std::cerr << "mysql connection error" << std::endl;
                root["error"] = ErrorCodes::ERR_MYSQL;
                break;
            case -2:
                std::cerr << "user is nonexistent" << std::endl;
                root["error"] = ErrorCodes::UserNotExist;
                break;
            case -3:
                std::cerr << "password is error" << std::endl;
                root["error"] = ErrorCodes::PasswordError;
                break;
            default:
                root["error"]=ErrorCodes::SUCCESS;
        }
        root["user"] = user;
        root["password"] = password;
        auto json_str = root.dump(4);
        std::cout << "sent json is:" << json_str << std::endl;
        boost::beast::ostream(connection->_response.body()) << json_str;
        return true;
    });

    //邮箱登录
    regPost("/email_login", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data()); //获取body
        std::cout << "receive body is:" << body_str << std::endl;
        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        json src_root = json::parse(body_str); //接收的json
        json root; //发送的json
        if (src_root.is_null()) {
            std::cout << "Failed to parse JSON data" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string json_str = root.dump(4);
            std::cout << "sent json is:" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return true;
        }

        std::string email = src_root["email"];
        std::string password = src_root["password"];
        int result = MySQLMgr::getInstance()->loginEmail(email, password);
        switch (result) {
            case -1:
                std::cerr << "mysql connection error" << std::endl;
                root["error"] = ErrorCodes::ERR_MYSQL;
                break;
            case -2:
                std::cerr << "email is nonexistent" << std::endl;
                root["error"] = ErrorCodes::EmailNotExist;
                break;
            case -3:
                std::cerr << "password is error" << std::endl;
                root["error"] = ErrorCodes::PasswordError;
                break;
            default:
                root["error"]=ErrorCodes::SUCCESS;
        }
        root["email"] = email;
        root["password"] = password;
        auto json_str = root.dump(4);
        std::cout << "sent json is:" << json_str << std::endl;
        boost::beast::ostream(connection->_response.body()) << json_str;
        return true;
    });
}

void LogicSystem::regPost(std::string url, HttpHandler handler) {
    _postHandlers.insert({url, handler});
}
