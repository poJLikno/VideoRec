#ifndef DOUBLE_BUFFER_H_
#define DOUBLE_BUFFER_H_

#include <stdint.h>

class DoubleBuffer
{
protected:
    bool _indexes_busy[2] = {0};
    uint8_t _lock_index = 0;/* For GetBuff method */
    uint8_t _newest_index = 1;

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

