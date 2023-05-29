#ifndef __SHAREDQUEUE_H
#define __SHAREDQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * @brief 线程安全的共享队列
 */
template <typename T>
class SharedQueue
{
public:
    SharedQueue();
    ~SharedQueue();

    void setDepth(size_t depth);

    T &front();
    void pop();

    void push(const T &item);
    void push(T &&item);

    size_t size();
    bool empty();

private:
    std::deque<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
    size_t depth = 0;
};

template <typename T>
SharedQueue<T>::SharedQueue()
{
    this->depth = queue_.max_size();
};

template <typename T>
SharedQueue<T>::~SharedQueue() {}


template <typename T>
void SharedQueue<T>::setDepth(size_t depth)
{
    this->depth = depth;
}

template <typename T>
bool SharedQueue<T>::empty()
{
    return size() ? false : true;
}

template <typename T>
T &SharedQueue<T>::front()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
        cond_.wait(mlock);
    }
    return queue_.front();
}

template <typename T>
void SharedQueue<T>::pop()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
        cond_.wait(mlock);
    }
    queue_.pop_front();
}

template <typename T>
void SharedQueue<T>::push(const T &item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(item);
    if (queue_.size() > this->depth)
    {
        queue_.pop_front();
    }
    mlock.unlock();     // unlock before notificiation to minimize mutex con
    cond_.notify_one(); // notify one waiting thread
}

template <typename T>
void SharedQueue<T>::push(T &&item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(std::move(item));
    if (queue_.size() > this->depth)
    {
        queue_.pop_front();
    }
    mlock.unlock();     // unlock before notificiation to minimize mutex con
    cond_.notify_one(); // notify one waiting thread
}

template <typename T>
size_t SharedQueue<T>::size()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    size_t size = queue_.size();
    mlock.unlock();
    return size;
}

#endif