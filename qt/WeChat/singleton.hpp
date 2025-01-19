#ifndef SINGLETON_HPP
#define SINGLETON_HPP
/******************************************************************************
 *
 * @file       singleton.hpp
 * @brief      单例模式模板
 *
 * @author     李佳承
 * @date       2024/11/01
 * @history
 *****************************************************************************/
#include "global.h"

template<class T>
class Singleton:public QObject
{
public:
    //获取单例
    static std::shared_ptr<T> getInstance(){
        static std::shared_ptr<T> _instance=std::shared_ptr<T>(new T);
        return _instance;
    }

    //析构函数
    ~Singleton(){
        qDebug()<<"Singleton调用了析构函数";
    }
protected:
    //禁用构造函数
    Singleton(){}
    //禁用拷贝构造函数
    Singleton(const Singleton<T>&)=delete;
    //禁用赋值构造函数
    Singleton& operator=(const Singleton<T>&)=delete;
};

#endif // SINGLETON_HPP
