#ifndef GATESERVER_LOGICSYSTEM_H
#define GATESERVER_LOGICSYSTEM_H

#include "const.h"

class CSession;
class LogicNode;

typedef std::function<void(const std::string&, std::shared_ptr<CSession>)> HttpHandler;

class LogicSystem : public Singleton<LogicSystem> {
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();
    bool handleMsg(std::shared_ptr<LogicNode> logicNode);
    void postMsgToQueue(std::shared_ptr<LogicNode> logicNode);

private:
    LogicSystem();
    LogicSystem(const LogicSystem&) = delete;
    LogicSystem& operator=(const LogicSystem&) = delete;
    void close();

    void workerThread(int queueIndex);

    std::unordered_map<short, HttpHandler> _msgHandlers;
    
    const size_t _workerCount;
    std::vector<std::queue<std::shared_ptr<LogicNode>>> _queues;
    std::vector<std::mutex> _queueMutexes;
    std::vector<std::condition_variable> _queueCvs;
    std::vector<std::thread> _threads;
    std::atomic<bool> _isStop;

    size_t getQueueIndex(int uid) const;
};

#endif //GATESERVER_LOGICSYSTEM_H