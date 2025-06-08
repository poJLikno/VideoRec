#ifndef DOUBLE_BUFFER_H_
#define DOUBLE_BUFFER_H_

#include <stdint.h>

#include <atomic>


class DoubleBuffer
{
protected:
    class SpinLock
    {
    private:
        std::atomic<bool> _lock = { false };

    public:
        SpinLock() = default;
        SpinLock(const SpinLock &) = delete;
        SpinLock(SpinLock &&) = delete;

        ~SpinLock();

        void Lock();
        bool TryLock();
        void Unlock();
    };

    SpinLock _indexes_busy[2] = { };
    std::atomic<uint8_t> _newest_index = { 1 };
    uint8_t _lock_index = 0;

    virtual void _OnWrite(const uint8_t &index) = 0;

public:
    DoubleBuffer() = default;
    DoubleBuffer(const DoubleBuffer &) = delete;
    virtual ~DoubleBuffer() = default;

    virtual void Lock() final;/* Call before GetBuff method */
    virtual void Unlock() final;/* Call after GetBuff method */

    virtual void Write() final;
};

#endif /* DOUBLE_BUFFER_H_ */

