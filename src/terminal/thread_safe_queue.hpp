#pragma once

#include <mutex>
#include <queue>

template <typename T>
class ThreadSafeQueue {
public:
    void push(T&& value) {
        std::scoped_lock<std::mutex> lock(m_Mutex);
        m_Queue.push(value);
    }

    /// Returns true if popped, false if queue was empty
    bool pop(T& out) {
        std::scoped_lock<std::mutex> lock(m_Mutex);
        if (m_Queue.empty()) {
            return false;
        }

        out = std::move(m_Queue.front());
        m_Queue.pop();
        return true;
    }

    size_t size() const {
        std::scoped_lock<std::mutex> lock(m_Mutex);
        return m_Queue.size();
    }

private:
    std::queue<T> m_Queue;
    mutable std::mutex m_Mutex;
};