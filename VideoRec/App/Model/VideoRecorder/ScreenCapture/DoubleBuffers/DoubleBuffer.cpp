#include "DoubleBuffer.h"

#include <string>

void DoubleBuffer::Lock()
{
    uint8_t local_newest_index = _newest_index;
    if (_indexes_busy[local_newest_index] == false)
    {
        _indexes_busy[local_newest_index] = true;

        _lock_index = local_newest_index;
    }
    else
    {
        while (_indexes_busy[(local_newest_index ^ (1 << 0))] == true);
        _indexes_busy[(local_newest_index ^ (1 << 0))] = true;

        _lock_index = local_newest_index ^ (1 << 0);
    }
}

void DoubleBuffer::Unlock()
{
    _indexes_busy[_lock_index] = false;
}

void DoubleBuffer::Write()
{
    if (_indexes_busy[(_newest_index ^ (1 << 0))] == false)
    {
        _indexes_busy[(_newest_index ^ (1 << 0))] = true;

        _OnWrite((_newest_index ^ (1 << 0)));

        _newest_index ^= (1 << 0);
        _indexes_busy[_newest_index] = false;
    }
    else
    {
        while (_indexes_busy[_newest_index] == true);
        _indexes_busy[_newest_index] = true;

        _OnWrite(_newest_index);

        _indexes_busy[_newest_index] = false;
    }
}
