#include "SafeQueue.h"

template<typename T>
SafeQueue<T>::SafeQueue(const SafeQueue<T>& other) {
    std::lock_guard<std::mutex> lock(other.m);
    data = other.data;
}

template<typename T>
void SafeQueue<T>::push(T item) {
    std::lock_guard<std::mutex> lock(m);
    data.push(item);
    data_cond.notify_one();
}

template<typename T>
std::shared_ptr<T> SafeQueue<T>::wait_and_pop() {
    std::unique_lock<std::mutex> lock(m);
    data_cond.wait(lock,[this]{return !data.empty();});
    std::shared_ptr<T> item(std::make_shared<T>(data.front()));
    data.pop();
    return item;
}

template<typename T>
void SafeQueue<T>::wait_and_pop(T& value) {
    std::unique_lock<std::mutex> lock(m);
    data_cond.wait(lock,[this]{return !data.empty();});
    value = data.front();
    data.pop();
}

template<typename T>
bool SafeQueue<T>::try_pop(T& value) {
    std::lock_guard<std::mutex> lock(m);
    if (data.empty())
        return false;
    value = data.front();
    data.pop();
    return true;
}

template<typename T>
std::shared_ptr<T> SafeQueue<T>::try_pop() {
    std::lock_guard<std::mutex> lock(m);
    if (data.empty())
        return std::shared_ptr<T>();
    std::shared_ptr<T> item(std::make_shared<T>(data.front()));
    data.pop();
    return item;
}

template<typename T>
bool SafeQueue<T>::empty() const {
    std::lock_guard<std::mutex> lock(m);
    return data.empty();
}


