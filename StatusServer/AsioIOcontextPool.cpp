//
// Created by Beyond on 2024/11/23.
//

#include "AsioIOcontextPool.h"
#include <stdexcept>
#include <thread>
#include <iostream>

/**
 * ASIO IO上下文池实现，用于高效网络通信
 */

AsioIOcontextPool::AsioIOcontextPool(std::size_t pool_size)
    : _next_io_context(0)
{

    if (pool_size == 0)
    {
        throw std::invalid_argument("IO上下文池大小不能为0");
    }

    // 创建指定数量的IO上下文
    for (std::size_t i = 0; i < pool_size; ++i)
    {
        io_context_ptr context_ptr(new boost::asio::io_context);
        _io_contexts.push_back(context_ptr);
        _work.push_back(boost::asio::make_work_guard(*context_ptr));
    }
}

void AsioIOcontextPool::run()
{
    // 创建线程池来运行所有io_context
    std::vector<std::thread> threads;
    for (std::size_t i = 0; i < _io_contexts.size(); ++i)
    {
        threads.emplace_back([this, i]()
                             {
            try {
// 设置线程名称用于调试（平台特定）
#if defined(__linux__)
                pthread_setname_np(pthread_self(), ("io_worker_" + std::to_string(i)).c_str());
#endif
                
                _io_contexts[i]->run();
            } catch (const std::exception& e) {
                std::cerr << "IO上下文线程异常: " << e.what() << std::endl;
            } });
    }

    // 等待所有线程退出
    for (auto &thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

void AsioIOcontextPool::stop()
{
    // 显式停止所有io_context
    for (auto &context : _io_contexts)
    {
        context->stop();
    }
}

boost::asio::io_context &AsioIOcontextPool::getIOcontext()
{
    // 使用循环方式选择下一个要使用的io_context
    boost::asio::io_context &io_context = *_io_contexts[_next_io_context];
    ++_next_io_context;
    if (_next_io_context >= _io_contexts.size())
    {
        _next_io_context = 0;
    }
    return io_context;
}

std::size_t AsioIOcontextPool::size() const
{
    return _io_contexts.size();
}
