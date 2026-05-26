#include "VerifyGrpcClient.h"
#include "LogicSystem.h"
#include "HttpConnection.h"
#include "MySQLMgr.h"
#include "RedisMgr.h"
#include "Crypto.h"
#include "StatusGrpcClient.h"

using json = nlohmann::json;

bool LogicSystem::handleGet(std::string path, std::shared_ptr<HttpConnection> connection) {
    auto iter = _getHandlers.find(path);
    if (iter == _getHandlers.end()) {
        return false;
    }
    iter->second(connection);
    return true;
}

bool LogicSystem::handlePost(std::string path, std::shared_ptr<HttpConnection> connection) {
    auto iter = _postHandlers.find(path);
    if (iter == _postHandlers.end()) {
        return false;
    }
    iter->second(connection);
    return true;
}

void LogicSystem::pushTask(std::string path, std::shared_ptr<HttpConnection> connection, bool isGet) {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_queue.size() > QUEUE_MAX_SIZE) {
            std::cerr << "queue is full, drop task!" << std::endl;
            return;
        }
        _queue.push({path, connection, isGet});
    }
    _cv.notify_one();
}

void LogicSystem::workerThread() {
    while (!_isStop) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cv.wait(lock, [this]() {
                return _isStop || !_queue.empty();
            });
            if (_isStop) {
                break;
            }
            if (!_queue.empty()) {
                task = std::move(_queue.front());
                _queue.pop();
            }
        }
        if (!task.connection) {
            continue;
        }
        bool result = false;
        if (task.isGet) {
            result = handleGet(task.path, task.connection);
        } else {
            result = handlePost(task.path, task.connection);
        }
        // 处理失败
        if (!result) {
            task.connection->_response.result(boost::beast::http::status::not_found);
            task.connection->_response.set(boost::beast::http::field::content_type, "text/plain");
            boost::beast::ostream(task.connection->_response.body()) << "url not found\r\n";
        } else {
            // 处理成功
            task.connection->_response.result(boost::beast::http::status::ok);
            task.connection->_response.set(boost::beast::http::field::server, "GateServer");
        }
        task.connection->response();
    }

    std::lock_guard<std::mutex> lock(_mutex);
    while (!_queue.empty()) {
        auto task = std::move(_queue.front());
        _queue.pop();
        if (task.connection) {
            bool result = false;
            if (task.isGet) {
                result = handleGet(task.path, task.connection);
            } else {
                result = handlePost(task.path, task.connection);
            }
            if (!result) {
                task.connection->_response.result(boost::beast::http::status::not_found);
                task.connection->_response.set(boost::beast::http::field::content_type, "text/plain");
                boost::beast::ostream(task.connection->_response.body()) << "url not found\r\n";
            } else {
                task.connection->_response.result(boost::beast::http::status::ok);
                task.connection->_response.set(boost::beast::http::field::server, "GateServer");
            }
            task.connection->response();
        }
    }
}

LogicSystem::~LogicSystem() {
    close();
}

void LogicSystem::close() {
    _isStop = true;
    _cv.notify_all();
    for (auto &thread : _threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void LogicSystem::regGet(std::string url, HttpHandler handler) {
    _getHandlers.insert({url, handler});
}

void LogicSystem::regPost(std::string url, HttpHandler handler) {
    _postHandlers.insert({url, handler});
}

LogicSystem::LogicSystem() : _isStop(false) {
    regGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        boost::beast::ostream(connection->_response.body()) << "receive get_test request";
    });

    regPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is:" << body_str << std::endl;
        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        json src_root = json::parse(body_str);
        json root;
        if (src_root.is_null()) {
            std::cout << "Failed to parse JSON data" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string json_str = root.dump(4);
            boost::beast::ostream(connection->_response.body()) << json_str;
            return;
        }
        if (src_root.contains("email")) {
            const auto email = src_root["email"];
            std::cout << "email is:" << email << std::endl;
            root["email"] = email;
            auto codeReply = VerifyGrpcClient::getInstance()->GetVerifyCode(email);
            root["error"] = codeReply.error();
            root["code"] = codeReply.code();
            std::string json_str = root.dump(4);
            boost::beast::ostream(connection->_response.body()) << json_str;
            return;
        } else {
            std::cout << "email is inexistence" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string json_str = root.dump(4);
            boost::beast::ostream(connection->_response.body()) << json_str;
            return;
        }
    });

    regPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is:" << body_str << std::endl;
        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        json src_root = json::parse(body_str);
        json root;
        if (src_root.is_null()) {
            std::cout << "Failed to parse JSON data" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string json_str = root.dump(4);
            std::cout << "sent json is:" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return;
        }

        const std::string email = src_root["email"];
        std::string verifyCode = src_root["verifyCode"];
        std::string value;
        bool result_get = RedisMgr::getInstance()->get(CODE_PREFIX + email, value);
        if (result_get == false) {
            std::cout << "verifycode is nonexistent" << std::endl;
            root["error"] = ErrorCodes::VerifyCodeExpired;
            auto json_str = root.dump(4);
            std::cout << "sent json is" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return;
        }

        if (verifyCode != value) {
            std::cout << "verifycode is error" << std::endl;
            root["error"] = ErrorCodes::VerifyCodeError;
            auto json_str = root.dump(4);
            std::cout << "sent json is:" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return;
        }
        std::cout << "verifycode is right" << std::endl;

        const std::string name = src_root["user"];
        const std::string password = src_root["password"];
        auto result = MySQLMgr::getInstance()->regUser(name, password, email);
        std::cout << "regUser result is:" << result << std::endl;
        switch (result) {
            case -1:
                root["error"] = ErrorCodes::ERR_NETWORK;
                break;
            case -2:
                root["error"] = ErrorCodes::UserExist;
                break;
            case -3:
                root["error"] = ErrorCodes::EmailExist;
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
    });

    regPost("/reset_password", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is:" << body_str << std::endl;
        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        json src_root = json::parse(body_str);
        json root;
        if (src_root.is_null()) {
            std::cout << "Failed to parse JSON data" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string json_str = root.dump(4);
            std::cout << "sent json is:" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return;
        }

        const std::string email = src_root["email"];
        std::string verifyCode = src_root["verifyCode"];
        std::string value;
        bool result_get = RedisMgr::getInstance()->get(CODE_PREFIX + email, value);
        if (result_get == false) {
            std::cout << "verifycode is nonexistent" << std::endl;
            root["error"] = ErrorCodes::VerifyCodeExpired;
            auto json_str = root.dump(4);
            std::cout << "sent json is" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return;
        }

        if (verifyCode != value) {
            std::cout << "verifycode is error" << std::endl;
            root["error"] = ErrorCodes::VerifyCodeError;
            auto json_str = root.dump(4);
            std::cout << "sent json is:" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return;
        }
        std::cout << "verifycode is right" << std::endl;

        const std::string password = src_root["password"];
        auto result = MySQLMgr::getInstance()->resetPassword(email, password);
        std::cout << "resetPassword result is:" << result << std::endl;
        switch (result) {
            case -1:
                root["error"] = ErrorCodes::ERR_NETWORK;
                break;
            case -2:
                root["error"] = ErrorCodes::EmailNotExist;
                break;
            case -3:
                root["error"] = ErrorCodes::PasswordSame;
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
    });

    regPost("/account_login", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is:" << body_str << std::endl;
        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        json src_root = json::parse(body_str);
        json root;
        if (src_root.is_null()) {
            std::cout << "Failed to parse JSON data" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string json_str = root.dump(4);
            std::cout << "sent json is:" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return;
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
                root["error"] = ErrorCodes::SUCCESS;
        }

        if (root["error"] == ErrorCodes::SUCCESS) {
            const int uid = MySQLMgr::getInstance()->getUID(1, user);
            root["uid"] = uid;
            auto respond = StatusGrpcClient::getInstance()->getChatServer(uid);
            if (respond.error() != ErrorCodes::SUCCESS) {
                root["error"] = respond.error();
            } else {
                root["token"] = respond.token();
                root["host"] = respond.host();
                root["port"] = respond.port();
            }
        }

        root["user"] = user;
        root["password"] = password;
        auto json_str = root.dump(4);
        std::cout << "sent json is:" << json_str << std::endl;
        boost::beast::ostream(connection->_response.body()) << json_str;
    });

    regPost("/email_login", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is:" << body_str << std::endl;
        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        json src_root = json::parse(body_str);
        json root;
        if (src_root.is_null()) {
            std::cout << "Failed to parse JSON data" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string json_str = root.dump(4);
            std::cout << "sent json is:" << json_str << std::endl;
            boost::beast::ostream(connection->_response.body()) << json_str;
            return;
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
                root["error"] = ErrorCodes::SUCCESS;
        }

        if (root["error"] == ErrorCodes::SUCCESS) {
            const int uid = MySQLMgr::getInstance()->getUID(0, email);
            root["uid"] = uid;
            auto respond = StatusGrpcClient::getInstance()->getChatServer(uid);
            if (respond.error() != ErrorCodes::SUCCESS) {
                root["error"] = respond.error();
            } else {
                root["token"] = respond.token();
                root["host"] = respond.host();
                root["port"] = respond.port();
            }
        }

        root["email"] = email;
        root["password"] = password;
        auto json_str = root.dump(4);
        std::cout << "sent json is:" << json_str << std::endl;
        boost::beast::ostream(connection->_response.body()) << json_str;
    });

    size_t workerCount = std::thread::hardware_concurrency();
    if (workerCount == 0) {
        workerCount = 4;
    }
    for (size_t i = 0; i < workerCount; ++i) {
        _threads.emplace_back(&LogicSystem::workerThread, this);
    }
}