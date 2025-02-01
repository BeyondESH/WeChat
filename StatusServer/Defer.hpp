//
// Created by Beyond on 2025/1/26.
//

#ifndef DEFER_HPP
#define DEFER_HPP

#include <functional>

class Defer {
public:
    Defer(std::function<void()> func):_func(func) {}
    ~Defer() {
        _func();
    }
private:
    std::function<void()> _func;
};



#endif //DEFER_HPP
