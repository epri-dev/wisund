#ifndef SAFEDEQUE_H
#define SAFEDEQUE_H
#include <exception>
#include <memory>
#include <queue>
#include <mutex>

struct EmptyQueue: std::exception {
    const char * what() const throw();
};

template<typename T>
class SafeDeque {
public:
    SafeDeque() = default;
    SafeDeque(const SafeDeque&);
    SafeDeque& operator=(const SafeDeque&) = delete;
    void push(T item);
    std::shared_ptr<T> pop();
    void pop(T& value);
    bool empty() const;
private:
    std::queue<T> data;
    mutable std::mutex m;
};
#endif // SAFEDEQUE_H
