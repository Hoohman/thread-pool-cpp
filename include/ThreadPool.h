#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <stop_token>
#include <thread>
#include <type_traits>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(std::size_t workers);
    ~ThreadPool();

    template <class F, class... Args>
    auto Enqueue(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    void Shutdown();

private:
    void working_loop(std::stop_token stop_token);

    std::mutex mutable mutex_;
    std::condition_variable_any cv_;
    bool working_ = false;
    std::queue<std::function<void()>> tasks_;
    std::vector<std::jthread> workers_;
};

template <class F, class... Args>
auto ThreadPool::Enqueue(F&& f, Args&&... args)
    -> std::future<std::invoke_result_t<F, Args...>> {
    using result_t = std::invoke_result_t<F, Args...>;

    auto task = std::make_shared<std::packaged_task<result_t()>>(
        [f = std::forward<F>(f),
         ... args = std::forward<Args>(args)]() mutable {
            return std::invoke(std::move(f), std::move(args)...);
        });

    std::future<result_t> result = task->get_future();

    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!working_) {
            throw std::runtime_error("Enqueue on stopped ThreadPool");
        }
        tasks_.emplace([task]() { (*task)(); });
    }

    cv_.notify_one();
    return result;
}