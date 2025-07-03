//
// Created by baikaishui on 2025/6/23.
//

#include <memory>
#include "asio_iocontext_pool.h"

AsioIOContextPool::AsioIOContextPool(std::size_t size) :
    io_services_(size), works_(size), next_io_service_(0) {
    // work的作用是让空的io_context去等待
    for (std::size_t i = 0; i < size; ++i) {
        works_[i] = std::make_unique<Work>(io_services_[i].get_executor());
    }
    // 多线程
    for (std::size_t i = 0; i < io_services_.size(); ++i) {
        threads_.emplace_back([this, i]() {
            io_services_[i].run();
        });
    }
}

AsioIOContextPool::~AsioIOContextPool() {
    stop();
    std::cout << "asio_io_context_pool.cpp: AsioIOContextPool destrcution" << std::endl;
}

boost::asio::io_context &AsioIOContextPool::getIOContext() {
    auto &service = io_services_[next_io_service_];
    next_io_service_ = (next_io_service_ + 1) % io_services_.size();
    return service;
}

void AsioIOContextPool::stop() {
    // work.reset()并不能将io_context停止运行
    // 需要手动停止
    for (size_t i = 0; i < io_services_.size(); ++i) {
        io_services_[i].stop();
        works_[i].reset();
    }
    // 等待所有线程结束
    for (auto &thread : threads_) {
        thread.join();
    }
}
