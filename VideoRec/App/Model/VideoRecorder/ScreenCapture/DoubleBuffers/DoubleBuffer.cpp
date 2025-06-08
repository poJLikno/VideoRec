#include "DoubleBuffer.h"

#include <string>
#include <thread>
#include <chrono>

void DoubleBuffer::Lock()
{
    uint8_t local_newest_index = _newest_index.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if (_indexes_busy[local_newest_index].TryLock())
    {
        _lock_index = local_newest_index;
    }
    else
    {
        _indexes_busy[local_newest_index ^ (1 << 0)].Lock();

        _lock_index = local_newest_index ^ (1 << 0);
    }
}

void DoubleBuffer::Unlock()
{
    _indexes_busy[_lock_index].Unlock();
}

void DoubleBuffer::Write()
{
    uint8_t local_newest_index = _newest_index.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    uint8_t neg_local_newest_index = local_newest_index ^ (1 << 0);
    if (_indexes_busy[neg_local_newest_index].TryLock())
    {
        _OnWrite(neg_local_newest_index);

        std::atomic_thread_fence(std::memory_order_release);
        _newest_index.store(neg_local_newest_index, std::memory_order_relaxed);
    }
    else
    {
        _indexes_busy[local_newest_index].Lock();

        _OnWrite(local_newest_index);
    }

    _indexes_busy[_newest_index].Unlock();
}

DoubleBuffer::SpinLock::~SpinLock()
{
    Unlock();
}

void DoubleBuffer::SpinLock::Lock()
{
    bool expected = false;
    bool tmp = false;
    do
    {
        expected = false;
        tmp = !_lock.compare_exchange_weak(expected, true, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
    } while (tmp);
}

bool DoubleBuffer::SpinLock::TryLock()
{
    bool expected = false;
    bool tmp = _lock.compare_exchange_strong(expected, true, std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if (tmp)
    {
        return true;
    }

    return false;
}

void DoubleBuffer::SpinLock::Unlock()
{
    std::atomic_thread_fence(std::memory_order_release);
    _lock.store(false, std::memory_order_relaxed);
}
