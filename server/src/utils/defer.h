//
// Created by baikaishui on 2025/7/1.
//

#ifndef DEFER_H
#define DEFER_H
#include <functional>

// 模拟go的defer功能来实现RAII
class Defer {
public:
    Defer(std::function<void()> func);
    ~Defer();
private:
    std::function<void()> defer_func_;
};



#endif //DEFER_H
