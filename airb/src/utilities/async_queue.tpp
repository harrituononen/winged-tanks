#include <cassert>

namespace utilities {

template<class T>
AsyncQueue<T>::AsyncQueue()
    : m_queue()
    , m_mutex()
    , m_cond_var()
{ }

template<class T>
AsyncQueue<T>::~AsyncQueue()
{ }

template<class T>
void AsyncQueue<T>::enqueue(T t)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(t);
    m_cond_var.notify_one();
}

template<class T>
T AsyncQueue<T>::dequeue()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while(m_queue.empty())
    {
        m_cond_var.wait(lock);
    }
    assert(!m_queue.empty());
    T val = m_queue.front();
    m_queue.pop();
    return val;
}

template<class T>
bool AsyncQueue<T>::empty() const
{
    return m_queue.empty();
}

} // namespace utilities
