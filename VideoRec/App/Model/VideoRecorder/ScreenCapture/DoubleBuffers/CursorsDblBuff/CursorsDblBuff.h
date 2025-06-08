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

    HWND _hwnd = nullptr;
    const bool _is_window_flag;
    const bool _client_rect_only_flag;
    const int _dpi;

    virtual void _OnWrite(const uint8_t &index) override final;

public:
    CursorsDblBuff(HWND hwnd, const bool &is_window, const bool &client_rect_only, const int &dpi);
    CursorsDblBuff(const CursorsDblBuff &) = delete;
    virtual ~CursorsDblBuff() override = default;

    virtual Cursor *GetCursor() final;
};

#endif /* CURSORS_DOUBLE_BUFFER_H_ */
