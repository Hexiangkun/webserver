#ifndef LOCK_H
#define LOCK_H

#include "noncopyable.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>     //uint32_t
#include <stdexcept>


/// @brief 封装Semphore/mutex
namespace hxk
{

class Semphore : public noncopyable
{
public:
    explicit Semphore(uint32_t count = 0)
    {
        if(sem_init(&m_semphore, 0, count) == -1)
        {
            throw std::logic_error("sem_init error");
        }
    }

    ~Semphore()
    {
        sem_destroy(&m_semphore);
    }

    /// @brief +1操作
    void notify()
    {
        if(sem_post(&m_semphore) == -1)
        {
            throw std::logic_error("sem_post error");
        }
    }

    /// @brief -1操作
    void wait()
    {
        if(sem_wait(&m_semphore) == -1)
        {
            throw std::logic_error("sem_wait error");
        }
    }
private:
    sem_t m_semphore;
};

/// @brief 作用域锁
/// @tparam T 锁的类型
template<typename T>
class ScopedLockImpl
{
public:
    explicit ScopedLockImpl(T* mutex):m_mutex(mutex), m_locked(false)
    {
        m_mutex->lock();
        m_locked = true;
    }

    ~ScopedLockImpl()
    {
        unlock();
    }

    void lock()
    {
        if(!m_locked) {
            m_mutex->lock();
            m_locked = true;
        }
    }
    void unlock()
    {
        if(m_locked) {
            m_locked = false;
            m_mutex->unlock();
        }
    }

private:
    T* m_mutex;     //锁
    bool m_locked;  //是否上锁
};

/// @brief 作用域读锁
/// @tparam T 读写锁
template<typename T>
class ReadScopedLockImpl
{
public:
    explicit ReadScopedLockImpl(T* mutex) : m_mutex(mutex), m_locked(false)
    {
        m_mutex->readLock();
        m_locked = true;
    }

    ~ReadScopedLockImpl()
    {
        unlock();
    }

    void lock()
    {
        if(!m_locked) {
            m_mutex->readLock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if(m_locked) {
            m_locked = false;
            m_mutex->unlock();
        }
    }
private:
    T* m_mutex;
    bool m_locked;
};

/// @brief 作用域写锁
/// @tparam T 读写锁
template<typename T>
class WriteScopedLockImpl
{
public:
    explicit WriteScopedLockImpl(T* mutex) : m_mutex(mutex), m_locked(false)
    {
        m_mutex->writeLock();
        m_locked = true;
    }

    ~WriteScopedLockImpl()
    {
        unlock();
    }

    void lock()
    {
        if(!m_locked) {
            m_mutex->writeLock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if(m_locked) {
            m_locked = false;
            m_mutex->unlock();
        }
    }
private:
    T* m_mutex;
    bool m_locked;
};

/// @brief 读写锁
class RWLock
{
public:
    RWLock()
    {
        //成功返回0，失败返回其他
        if(pthread_rwlock_init(&m_lock, nullptr)) {
            throw std::logic_error("pthread_rwlock_init error");
        }
    }
    ~RWLock()
    {
        if(pthread_rwlock_destroy(&m_lock)) {
            throw std::logic_error("pthread_rwlock_destory error");
        }
    }

    int readLock()
    {
        return pthread_rwlock_rdlock(&m_lock);
    }

    int writeLock()
    {
        return pthread_rwlock_wrlock(&m_lock);
    }

    int unlock()
    {
        return pthread_rwlock_unlock(&m_lock);
    }

private:
    pthread_rwlock_t m_lock;
};


class Mutex
{
public:
    Mutex()
    {
        pthread_mutex_init(&m_mutex, nullptr);
    }
    ~Mutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    int lock()
    {
        return pthread_mutex_lock(&m_mutex);
    }

    int unlock()
    {
        return pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

/// @brief 作用域锁包装
using ScopedLock = ScopedLockImpl<Mutex>;

/// @brief 读锁包装
using ReadScopedLock = ReadScopedLockImpl<RWLock>;

/// @brief 写锁包装
using WriteScopedLock = WriteScopedLockImpl<RWLock>;
}

#endif