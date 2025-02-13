#ifndef CURSORS_DOUBLE_BUFFER_H_
#define CURSORS_DOUBLE_BUFFER_H_

#include <windows.h>

#include "../DoubleBuffer.h"

class Cursor
{
public:
    HCURSOR h_cursor = nullptr;
    int relative_pos_x = 0;
    int relative_pos_y = 0;
    int width = 0;
    int height = 0;
};

class CursorsDblBuff : public DoubleBuffer
{
protected:
    Cursor _cursors_buffers[2] = { 0 };
    Cursor _staged_cursor = { 0 };

    virtual void _OnWrite(const uint8_t &index) override final;

public:
    CursorsDblBuff() = default;
    CursorsDblBuff(const CursorsDblBuff &) = delete;
    virtual ~CursorsDblBuff() override = default;

    virtual Cursor *GetCursor() final;
    virtual void SetCursor(const Cursor &cursor) final;
};

#endif /* CURSORS_DOUBLE_BUFFER_H_ */
