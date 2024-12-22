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
        static std::once_flag flag;
        std::call_once(flag, [&]() {
            _instance = std::shared_ptr<T>(new T);
        });
        return _instance;
    }

    void printAddress() {
        std::cout << _instance.get() << std::endl;
    }

    ~Singleton() {
        std::cout << "单例被析构" << std::endl;
    }

protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;
    static std::shared_ptr<T> _instance;
};

template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;

#endif //GATESERVER_SINGLETON_CPP