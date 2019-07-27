#ifndef UTILITIES_ASYNC_QUEUE_HPP
#define UTILITIES_ASYNC_QUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <queue>

namespace utilities {

template <class T>
class AsyncQueue final
{
private:
    std::queue<T>           m_queue;
    mutable std::mutex      m_mutex;
    std::condition_variable m_cond_var;
public:
     AsyncQueue();
    ~AsyncQueue();

    void enqueue(T t);
    T    dequeue();

    bool empty() const;
};

} // namespace utilities

#include "async_queue.tpp"

#endif // UTILITIES_ASYNC_QUEUE_HPP
