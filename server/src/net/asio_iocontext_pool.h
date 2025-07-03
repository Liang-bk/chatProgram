//
// Created by baikaishui on 2025/6/23.
//

#ifndef ASIO_IOCONTEXT_POOL_H
#define ASIO_IOCONTEXT_POOL_H
#include <boost/asio/io_context.hpp>
#include "common/singleton.h"

class AsioIOContextPool : public Singleton<AsioIOContextPool> {
    friend class Singleton<AsioIOContextPool>;
public:
    using IOService = boost::asio::io_context;
    using Work = boost::asio::executor_work_guard<IOService::executor_type>;
    using WorkPtr = std::unique_ptr<Work>;
    ~AsioIOContextPool();
    // 禁用拷贝构造
    AsioIOContextPool(const AsioIOContextPool&) = delete;
    // 禁用赋值
    AsioIOContextPool& operator=(const AsioIOContextPool&) = delete;
    boost::asio::io_context& getIOContext();
    void stop();
private:
    AsioIOContextPool(std::size_t size = 2);
    std::vector<IOService> io_services_;
    std::vector<WorkPtr> works_;
    std::vector<std::thread> threads_;
    std::size_t next_io_service_;
};



#endif //ASIO_IOCONTEXT_POOL_H
