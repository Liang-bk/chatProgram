//
// Created by baikaishui on 2025/6/5.
//
#pragma once
#ifndef SINGLETON_H
#define SINGLETON_H
#include <memory>
#include <mutex>
#include <iostream>
#include "header/constant.h"
/******************************************************************************
 *
 * @file       singleton.h
 * @brief      单例基类, 懒汉模式, 方便后续子类继承使用
 *
 * @author     白开水
 * @date       2025/06/04
 * @history
 *****************************************************************************/

template<typename T>
class Singleton {
    // protect 使子类能够访问
protected:
    Singleton() = default;
    // 不允许拷贝赋值
    Singleton(const Singleton<T>&) = delete;
    Singleton operator = (const Singleton<T>& other) = delete;

    static std::shared_ptr<T> instance_;
public:
    static std::shared_ptr<T> getInstance() {
        // 只初始化1次
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            // 要用make_shared, 需要在子类中设置友元, 否则会报访问错误
            // 在子类中将Singleton类设为友元, 可以new子类, 否则也会报错
            instance_ = std::shared_ptr<T>(new T);
            });
        return instance_;
    }
    // 更简单的写法: (instance_就无用了)
    // static T& getInstance() {
    //     static T instance;
    //     return instance;
    // }

    void printAddress() {
        std::cout << instance_.get() << std::endl;
    }
    ~Singleton() {
        std::cout << "This is Singleton destruction." << std::endl;
    }

};
// 类内static变量初始化
template<typename T>
std::shared_ptr<T> Singleton<T>::instance_ = nullptr;
#endif // SINGLETON_H
