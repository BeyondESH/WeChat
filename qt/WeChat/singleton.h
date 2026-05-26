#ifndef SINGLETON_H
#define SINGLETON_H

template<typename T>
class Singleton
{
protected:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

public:
    static T* instance()
    {
        static T instance;
        return &instance;
    }
};

#endif // SINGLETON_H
