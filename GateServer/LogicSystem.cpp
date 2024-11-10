//
// Created by Beyond on 2024/11/10.
//

#include "LogicSystem.h"
#include "HttpConnection.h"

bool LogicSystem::handleGet(std::string path, std::shared_ptr<HttpConnection> connection) {
    //未找到path
    if(_getHandlers.find(path)==_getHandlers.end()){
        return false;
    }
    _getHandlers[path](connection);
    return true;
}

LogicSystem::~LogicSystem() {

}

void LogicSystem::regGet(std::string url, HttpHandler handler) {
    _getHandlers.insert({url,handler});
}

LogicSystem::LogicSystem(){
    regGet("/get_test",[](std::shared_ptr<HttpConnection> connection){
        boost::beast::ostream(connection->_response.body())<<"receive get_test request";
    });
}
