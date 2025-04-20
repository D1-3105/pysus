//
// Created by oleg on 20.04.25.
//

#ifndef SUSPENDER_THREAD_HOLDER_H
#define SUSPENDER_THREAD_HOLDER_H

#include "thread"
#include "mutex"
#include "queue"
#include "condition_variable"

namespace threader {
    template<typename T>
    class ThreadSafeQueue {
    public:
        void push(const T& value) {
            std::lock_guard<std::mutex> lock(mu_);
            queue_.push(value);
            cv_.notify_one();
        }

        T wait_and_pop() {
            std::unique_lock<std::mutex> lock(mu_);
            cv_.wait(lock, [this] { return !queue_.empty(); });
            T value = queue_.front();
            queue_.pop();
            return value;
        }

    private:
        mutable std::mutex mu_;
        std::queue<T> queue_;
        std::condition_variable cv_;
    };

    class ThreadHolder {
    private:
        std::thread* remote_thread_;
        std::thread* py_thread_;
        bool stop_;
    public:
        explicit ThreadHolder(const std::string& pyString);
        ~ThreadHolder();

        void callStop();
    };
}

#endif //SUSPENDER_THREAD_HOLDER_H
