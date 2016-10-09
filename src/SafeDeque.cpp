#include "SafeDeque.h"

template<typename T>
SafeDeque<T>::SafeDeque(const SafeDeque<T>& other) {
    std::lock_guard<std::mutex> lock(other.m);
    data = other.data;
}

template<typename T>
void SafeDeque<T>::push(T item) {
    std::lock_guard<std::mutex> lock(m);
    data.push(item);
}

template<typename T>
std::shared_ptr<T> SafeDeque<T>::pop() {
    std::lock_guard<std::mutex> lock(m);
    if (data.empty()) 
        throw EmptyQueue{};
    std::shared_ptr<T> const item(std::make_shared<T>(data.top));
    data.pop();
    return item;
}

template<typename T>
void SafeDeque<T>::pop(T& value) {
    std::lock_guard<std::mutex> lock(m);
    if (data.empty()) 
        throw EmptyQueue{};
    value = data.top();
    data.pop();
}

template<typename T>
bool SafeDeque<T>::empty() const {
    std::lock_guard<std::mutex> lock(m);
    return data.empty();
}


