#ifndef GATESERVER_LOGICSYSTEM_H
#define GATESERVER_LOGICSYSTEM_H

#include "const.h"

class HttpConnection;

typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem : public Singleton<LogicSystem> {
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();
    bool handleGet(std::string path, std::shared_ptr<HttpConnection> connection);
    bool handlePost(std::string path, std::shared_ptr<HttpConnection> connection);
    void regGet(std::string url, HttpHandler handler);
    void regPost(std::string url, HttpHandler handler);
    void pushTask(std::string path, std::shared_ptr<HttpConnection> connection, bool isGet);

private:
    LogicSystem();
    LogicSystem(const LogicSystem&) = delete;
    LogicSystem& operator=(const LogicSystem&) = delete;
    void close();
    void workerThread();

    std::map<std::string, HttpHandler> _getHandlers;
    std::map<std::string, HttpHandler> _postHandlers;

    struct Task {
        std::string path;
        std::shared_ptr<HttpConnection> connection;
        bool isGet;
    };

    std::queue<Task> _queue;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::atomic<bool> _isStop;
    std::vector<std::thread> _threads;
};

#endif //GATESERVER_LOGICSYSTEM_H