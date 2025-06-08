#include "CursorsDblBuff.h"

void CursorsDblBuff::_OnWrite(const uint8_t &index)
{
    memset(&_cursors_buffers[index], 0, sizeof(Cursor));

    if (_is_window_flag ? GetForegroundWindow() == _hwnd : 1)
    {
        CURSORINFO cursor_info = { 0 };
        cursor_info.cbSize = sizeof(CURSORINFO);
        GetCursorInfo(&cursor_info);

        if (cursor_info.flags & CURSOR_SHOWING)
        {
            ICONINFO icon_info = { 0 };
            GetIconInfo(cursor_info.hCursor, &icon_info);

            BITMAP bmp = { 0 };
            GetObject(icon_info.hbmColor, sizeof(BITMAP), (void *)&bmp);

            RECT rect = { 0 };
            if (_client_rect_only_flag)
            {
                ClientToScreen(_hwnd, (POINT *)&rect);
            }
            else
            {
                GetWindowRect(_hwnd, &rect);
            }

            _cursors_buffers[index].h_cursor = cursor_info.hCursor;
            _cursors_buffers[index].relative_pos_x = (cursor_info.ptScreenPos.x - (int)icon_info.xHotspot - rect.left) * _dpi / USER_DEFAULT_SCREEN_DPI;
            _cursors_buffers[index].relative_pos_y = (cursor_info.ptScreenPos.y - (int)icon_info.yHotspot - rect.top) * _dpi / USER_DEFAULT_SCREEN_DPI;
            _cursors_buffers[index].width = bmp.bmWidth * _dpi / USER_DEFAULT_SCREEN_DPI;
            _cursors_buffers[index].height = bmp.bmHeight * _dpi / USER_DEFAULT_SCREEN_DPI;

            DeleteObject(icon_info.hbmColor);
            DeleteObject(icon_info.hbmMask);
        }
    }
}

CursorsDblBuff::CursorsDblBuff(HWND hwnd, const bool &is_window, const bool &client_rect_only, const int &dpi)
    : _hwnd(hwnd), _is_window_flag(is_window), _client_rect_only_flag(client_rect_only), _dpi(dpi)
{}

Cursor *CursorsDblBuff::GetCursor()
{
    return &_cursors_buffers[_lock_index];
}
