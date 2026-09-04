#include "ThreadPool.h"
#include <cstddef>
#include <functional>
#include <mutex>
#include <stop_token>
#include <utility>

ThreadPool::ThreadPool(std::size_t workers) {
    workers_.reserve(workers);
    working_ = true;
    for (std::size_t i = 0; i < workers; i++) {
        workers_.emplace_back([this](std::stop_token stop_token) {
            this->working_loop(stop_token);
        });
    }
}

ThreadPool::~ThreadPool() { Shutdown(); }

void ThreadPool::Shutdown() {
    {
        std::unique_lock lock(mutex_);
        if (!working_) {
            return;
        }
        working_ = false;
    }
    for (auto& worker : workers_) {
        worker.request_stop();
    }
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::working_loop(std::stop_token stop_token) {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, stop_token,
                     [this]() { return !tasks_.empty(); });

            if (stop_token.stop_requested() && tasks_.empty()) {
                return;
            }

            task = std::move(tasks_.front());
            tasks_.pop();
        }
        task();
    }
}