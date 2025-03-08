//
// Created by Beyond on 2024/11/10.
//

#ifndef GATESERVER_LOGICSYSTEM_H
#define GATESERVER_LOGICSYSTEM_H

#include "const.h"
//提前声明
class CSession;
class LogicNode;
class MsgNode;

typedef std::function<void(std::shared_ptr<MsgNode>,std::shared_ptr<CSession>)>HttpHandler;

class LogicSystem:public Singleton<LogicSystem>{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();
    //处理消息
    bool handleMsg(std::shared_ptr<LogicNode> logicNode);
    void postMsgToQueue(std::shared_ptr<LogicNode> logicNode);
    // void regGet(std::string url,HttpHandler handler);
    // void regPost(std::string url,HttpHandler handler);
private:
    LogicSystem();
    LogicSystem(const LogicSystem&)=delete;
    LogicSystem&operator=(const LogicSystem&)=delete;
    void close();
    std::unordered_map<short,HttpHandler> _msgHandlers;
    std::mutex _mutex;
    std::queue<std::shared_ptr<LogicNode>> _queue;
    std::condition_variable _cv;
    std::atomic<bool> _isStop;
};


#endif //GATESERVER_LOGICSYSTEM_H
