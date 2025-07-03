//
// Created by baikaishui on 2025/7/3.
//
#include "utils/defer.h"
Defer::Defer(std::function<void()> func) {
    defer_func_ = func;
}
Defer::~Defer() {
    defer_func_();
}