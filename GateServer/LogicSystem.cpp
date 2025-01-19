//
// Created by Beyond on 2024/11/10.
//
#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"
#include "RedisMgr.h"
using json=nlohmann::json;
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
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());//获取body
        std::cout << "receive body is:" << body_str << std::endl;
        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        json src_root=json::parse(body_str);//接收的json
        json root;//发送的json
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
            auto codeReply=VerifyGrpcClient::getInstance()->GetVerifyCode(email);//调用grpc获取验证码
            root["error"] = codeReply.error();//错误码
            root["code"]=codeReply.code();//验证码
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

    regPost("/user_register",[](std::shared_ptr<HttpConnection> connection) {
        auto body_str=boost::beast::buffers_to_string(connection->_request.body().data());//获取body
        std::cout << "receive body is:" << body_str << std::endl;
        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        json src_root=json::parse(body_str);//接收的json
        json root;//发送的json
        if (src_root.is_null()) {
            std::cout << "Failed to parse JSON data" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string json_str = root.dump(4);
            boost::beast::ostream(connection->_response.body()) << json_str;
            return true;
        }

        //查找并验证redis中的验证码
        const std::string email=src_root["email"];
        std::string verifyCode=src_root["verifyCode"];
        std::string value;
        bool result_get=RedisMgr::getInstance()->get(CODE_PREFIX+email,value);
        //验证码过期或不存在
        if (result_get==false) {
            std::cout<<"verifycode is nonexistent"<<std::endl;
            root["error"]=ErrorCodes::VerifyCodeExpired;
            auto json_str=root.dump(4);
            boost::beast::ostream(connection->_response.body())<<json_str;
            return true;
        }

        //验证码错误
        if (verifyCode!=value) {
            std::cout<<"verifycode is error"<<std::endl;
            root["error"]=ErrorCodes::VerifyCodeError;
            auto json_str=root.dump(4);
            boost::beast::ostream(connection->_response.body())<<json_str;
            return true;
        }

        //查找MySQL数据库

        root["error"]=ErrorCodes::SUCCESS;
        root["email"]=email;
        root["verifyCode"]=verifyCode;
        root["user"]=src_root["user"];
        root["password"]=src_root["password"];
        auto json_str=root.dump(4);
        boost::beast::ostream(connection->_response.body())<<json_str;
        return true;
    });
}

void LogicSystem::regPost(std::string url, HttpHandler handler) {
    _postHandlers.insert({url, handler});
}
