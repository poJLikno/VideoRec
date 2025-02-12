#include "CursorsDblBuff.h"

void CursorsDblBuff::_OnWrite(const uint8_t &index)
{
    _cursors_buffers[index] = _staged_cursor;
}

Cursor *CursorsDblBuff::Get()
{
    return &_cursors_buffers[_lock_index];
}

void CursorsDblBuff::SetCursor(const Cursor &cursor)
{
    _staged_cursor = cursor;
}
