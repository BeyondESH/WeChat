//
// Created by Beyond on 2024/11/23.
//

#ifndef ASIOIOCONTEXTPOOL_H
#define ASIOIOCONTEXTPOOL_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <cstddef>
#include <thread>
#include <vector>
#include "singleton.hpp"
#include <memory>

class AsioIOcontextPool :public Singleton<AsioIOcontextPool>{
    friend class Singleton<AsioIOcontextPool>;
public:
    ~AsioIOcontextPool();
    boost::asio::io_context& getIOContext();//轮询返回一个空闲io_context
    void stop();
private:
    AsioIOcontextPool(std::size_t size=std::thread::hardware_concurrency());
    AsioIOcontextPool(const AsioIOcontextPool&)=delete;
    AsioIOcontextPool& operator=(const AsioIOcontextPool&)=delete;
    std::vector<boost::asio::io_context> _ioContexts;
    std::vector<std::thread> _threads;
    std::vector<std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>> _works;
    std::size_t _nextIOcontext;//轮询序号

};



#endif //ASIOIOCONTEXTPOOL_H
