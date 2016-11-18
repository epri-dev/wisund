#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H
#include <exception>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

struct EmptyQueue: std::exception {
    const char * what() const throw();
};

template<typename T>
class SafeQueue {
public:
    SafeQueue() = default;
    SafeQueue(const SafeQueue& other) {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.data;
    }
    SafeQueue& operator=(const SafeQueue&) = delete;
    void push(T item) {
        std::lock_guard<std::mutex> lock(m);
        data.push(item);
        data_cond.notify_one();
    }
    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> item(std::make_shared<T>(data.front()));
        data.pop();
        return item;
    }
    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty())
            return false;
        value = data.front();
        data.pop();
        return true;
    }
    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lock(m);
        data_cond.wait(lock,[this]{return !data.empty();});
        std::shared_ptr<T> item(std::make_shared<T>(data.front()));
        data.pop();
        return item;
    }
    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(m);
        data_cond.wait(lock,[this]{return !data.empty();});
        value = data.front();
        data.pop();
    }
    bool empty() const {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }


private:
    std::queue<T> data;
    mutable std::mutex m;
    std::condition_variable data_cond;
};
#endif // SAFEQUEUE_H
