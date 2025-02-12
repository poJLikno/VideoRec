#include "DoubleBuffer.h"

#include <string>

void DoubleBuffer::Lock()
{
    uint8_t local_newest_index = _newest_index;
    if (_mutexs[local_newest_index].try_lock())
    {
        _lock_index = local_newest_index;
    }
    else
    {
        _mutexs[(local_newest_index ^ (1 << 0))].lock();
        _lock_index = local_newest_index ^ (1 << 0);
    }
}

void DoubleBuffer::Unlock()
{
    _mutexs[_lock_index].unlock();
}

void DoubleBuffer::Write()
{
    if (_mutexs[(_newest_index ^ (1 << 0))].try_lock())
    {
        _OnWrite((_newest_index ^ (1 << 0)));

        _newest_index ^= (1 << 0);
        _mutexs[_newest_index].unlock();
    }
    else
    {
        _mutexs[_newest_index].lock();

        _OnWrite(_newest_index);

        _mutexs[_newest_index].unlock();
    }
}
