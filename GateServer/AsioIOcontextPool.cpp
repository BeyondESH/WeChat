//
// Created by Beyond on 2024/11/23.
//

#include "AsioIOcontextPool.h"

AsioIOcontextPool::AsioIOcontextPool(std::size_t size): _ioContexts(size), _works(size), _nextIOcontext(0) {
    //初始化works
    {
        std::size_t i = 0;
        for (auto &work: _works) {
            work = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(boost::asio::make_work_guard(_ioContexts[i++]));
        }
    }

    //初始化线程，运行io_context
    for (auto &io_context: _ioContexts) {
        _threads.emplace_back([&io_context]() {
            io_context.run();
        });
    }
}
AsioIOcontextPool::~AsioIOcontextPool() {
    stop();
    std::cout << "AsioIOcontextPool destruct" << std::endl;
}

boost::asio::io_context &AsioIOcontextPool::getIOContext() {
    auto &io_context = _ioContexts[_nextIOcontext++];
    if (_nextIOcontext == _ioContexts.size()) {
        _nextIOcontext = 0;
    }
    return io_context;
}

void AsioIOcontextPool::stop() {
    //停止所有io_context
    for (auto &io_context: _ioContexts) {
        io_context.stop();//停止io_context
    }
    //等待所有线程结束
    for (auto &thread: _threads) {
        thread.join();
    }
}


