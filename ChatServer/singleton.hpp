//
// Created by Beyond on 2024/11/10.
//

#ifndef GATESERVER_SINGLETON_CPP
#define GATESERVER_SINGLETON_CPP

#include <iostream>

template<class T>
class Singleton {
public:
    static std::shared_ptr<T> getInstance() {
        // 使用局部静态变量代替静态成员
        static std::shared_ptr<T> instance(new T);
        return instance;
    }

    void printAddress() {
        std::cout << getInstance().get() << std::endl;
    }

    ~Singleton() {
        std::cout << "单例被析构" << std::endl;
    }

protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;
};


#endif //GATESERVER_SINGLETON_CPP