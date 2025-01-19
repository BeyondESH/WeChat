//
// Created by Beyond on 2024/11/10.
//

#ifndef GATESERVER_LOGICSYSTEM_H
#define GATESERVER_LOGICSYSTEM_H

#include "const.h"
//提前声明
class HttpConnection;

typedef std::function<void(std::shared_ptr<HttpConnection>)>HttpHandler;

class LogicSystem:public Singleton<LogicSystem>{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();
    //处理get请求
    bool handleGet(std::string path,std::shared_ptr<HttpConnection> connection);
    bool handlePost(std::string path,std::shared_ptr<HttpConnection> connection);
    void regGet(std::string url,HttpHandler handler);
    void regPost(std::string url,HttpHandler handler);
private:
    LogicSystem();
    LogicSystem(const LogicSystem&)=delete;
    LogicSystem&operator=(const LogicSystem&)=delete;
    std::map<std::string,HttpHandler> _getHandlers;
    std::map<std::string,HttpHandler> _postHandlers;
};


#endif //GATESERVER_LOGICSYSTEM_H
