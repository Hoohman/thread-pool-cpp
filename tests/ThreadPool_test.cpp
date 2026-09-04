#include "ThreadPool.h"
#include <chrono>
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <thread>

int main() {
    ThreadPool tp{3};

    tp.Enqueue([]() {
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<int> distrib(100, 500);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(distrib(generator)));
        std::cout << "Thread id: " << std::this_thread::get_id() << " Task 1 "
                  << "No arguments" << '\n';
        std::cout << "Task 1 started\n";
        std::cout << "Task 1 finished\n";
    });

    auto result_int = tp.Enqueue(
        [](int num) {
            std::random_device rd;
            std::mt19937 generator(rd());
            std::uniform_int_distribution<int> distrib(100, 500);

            std::this_thread::sleep_for(
                std::chrono::milliseconds(distrib(generator)));
            std::cout << "Thread id: " << std::this_thread::get_id()
                      << " Task 2 "
                      << "Arguments: " << num << '\n';
            return num * num;
        },
        10);

    auto result_string = tp.Enqueue(
        [](std::string str, int count) {
            std::random_device rd;
            std::mt19937 generator(rd());
            std::uniform_int_distribution<int> distrib(100, 500);

            std::this_thread::sleep_for(
                std::chrono::milliseconds(distrib(generator)));
            std::cout << "Thread id: " << std::this_thread::get_id()
                      << " Task 3 "
                      << "Arguments: " << str << " " << count << '\n';
            std::string result;
            for (std::size_t i = 0; i < count; i++) {
                result.append(str).append(" ");
            }
            return result;
        },
        "Pam", 3);

    std::cout << "Result from task 2: " << result_int.get() << '\n';
    std::cout << "Result from task 3: " << result_string.get() << '\n';

    tp.Shutdown();

    return 0;
}