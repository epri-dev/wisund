#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H
#include <exception>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * Specialization of `std::exception` to handle an empty queue
 */
struct EmptyQueue: std::exception {
    /// returns the exception text
    const char * what() const throw();
};

/**
 * implementation of a thread-safe queue
 */
template<typename T>
class SafeQueue {
public:
    /// default constructor
    SafeQueue() = default;
    /// copy constructor 
    SafeQueue(const SafeQueue& other) {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.data;
    }
    /// delete the = constructor
    SafeQueue& operator=(const SafeQueue&) = delete;
    /// pushes copy of an item onto the queue
    void push(T item) {
        std::lock_guard<std::mutex> lock(m);
        data.push(item);
        data_cond.notify_one();
    }
    /// returns either a `shared_ptr` or `nullptr` if queue is empty
    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> item(std::make_shared<T>(data.front()));
        data.pop();
        return item;
    }
    /// returns true and populates passed reference only if the queue is not empty
    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty())
            return false;
        value = data.front();
        data.pop();
        return true;
    }
    /// waits for the queue to be non-empty and then pops that value, returning a shared pointer
    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lock(m);
        data_cond.wait(lock,[this]{return !data.empty();});
        std::shared_ptr<T> item(std::make_shared<T>(data.front()));
        data.pop();
        return item;
    }
    /// waits for the queue to be non-empty and then pops that value into passed reference
    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(m);
        data_cond.wait(lock,[this]{return !data.empty();});
        value = data.front();
        data.pop();
    }
    /// returns true if the queue is empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }


private:
    /// where the data is actually stored
    std::queue<T> data;
    /// mutex to insure integrity of the structure
    mutable std::mutex m;
    /// condition variable on which the various `wait...` functions rely
    std::condition_variable data_cond;
};
#endif // SAFEQUEUE_H
