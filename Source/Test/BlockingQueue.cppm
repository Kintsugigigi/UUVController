//
// @Time     : 2023/9/8 12:43
// @Author   : Yang
// @Software : CLion
//
module;
#include <queue>
#include <mutex>
#include <condition_variable>
//#include <assert.h>
#include <cassert>


export module BlockingQueueModule;

//export class BlockingQueue;


template <typename T>
class BlockingQueue
{
public:
    BlockingQueue()
            :m_mutex(),
             m_condition(),
             m_data(){}

    // 禁止拷贝构造
    BlockingQueue(BlockingQueue&) = delete;

    ~BlockingQueue(){}

    void push(T&& value)
    {
        // 往队列中塞数据前要先加锁
        std::unique_lock<std::mutex> lock(m_mutex);
        m_data.push(value);
        m_condition.notify_all();
    }

    void push(const T& value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_data.push(value);
        m_condition.notify_all();
    }

    T take()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while(m_data.empty())
        {
            m_condition.wait(lock);
        }
        assert(!m_data.empty());
        T value(std::move(m_data.front()));
        m_data.pop();

        return value;
    }

    size_t size() const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_data.size();
    }
private:
    // 实际使用的数据结构队列
    std::queue<T> m_data;

    // 条件变量的锁
    std::mutex m_mutex;
    std::condition_variable m_condition;
};
