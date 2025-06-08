#include "BitmapsDblBuff.h"

void BitmapsDblBuff::_OnWrite(const uint8_t &index)
{
    /* Get cursor if need */
    Cursor cursor = { 0 };

    if (_capture_cursor_flag)
    {
        if (_is_window_flag ? GetForegroundWindow() == _src_hwnd : true)
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
                    ClientToScreen(_src_hwnd, (POINT *)&rect);
                }
                else
                {
                    GetWindowRect(_src_hwnd, &rect);
                }


                cursor.h_cursor = cursor_info.hCursor;
                cursor.relative_pos_x = (cursor_info.ptScreenPos.x - (int)icon_info.xHotspot - rect.left) * _src_dpi / USER_DEFAULT_SCREEN_DPI;
                cursor.relative_pos_y = (cursor_info.ptScreenPos.y - (int)icon_info.yHotspot - rect.top) * _src_dpi / USER_DEFAULT_SCREEN_DPI;
                cursor.width = bmp.bmWidth * _src_dpi / USER_DEFAULT_SCREEN_DPI;
                cursor.height = bmp.bmHeight * _src_dpi / USER_DEFAULT_SCREEN_DPI;

                DeleteObject(icon_info.hbmColor);
                DeleteObject(icon_info.hbmMask);
            }
        }
    }

    /* Get captured frame */
    if (_use_optimization_flag)
    {
        BitBlt(_bitmaps_ctxs[index], 0, 0, _src_width, _src_height, _src_ctx, 0, 0, SRCCOPY);
    }
    else
    {
        PrintWindow(_src_hwnd, _bitmaps_ctxs[index], PW_RENDERFULLCONTENT);
    }

    /* Draw a cursor */
    if (cursor.h_cursor)
    {
        DrawIconEx(
            _bitmaps_ctxs[index],/* dst */
            cursor.relative_pos_x,/* x */
            cursor.relative_pos_y,/* y */
            cursor.h_cursor,/* src */
            cursor.width,/* width */
            cursor.height,/* height */
            0, NULL, DI_NORMAL);
    }

    _frames_dbl_buff->SetBuffer(&_dib_buffers[index]);
    _frames_dbl_buff->Write();
}

BitmapsDblBuff::BitmapsDblBuff(
    const HWND &src_hwnd, const HDC &src_ctx,
    const int &src_width, const int &src_height, const int &src_dpi,
    SmtObj<FramesDblBuff> &frames_dbl_buff,
    const bool &is_window, const bool &client_rect_only, const bool &use_optimization, const bool &capture_cursor)
    : _src_hwnd(src_hwnd), _src_ctx(src_ctx),
    _src_width(src_width), _src_height(src_height), _src_dpi(src_dpi),
    _frames_dbl_buff(frames_dbl_buff),
    _is_window_flag(is_window), _client_rect_only_flag(client_rect_only), _use_optimization_flag(use_optimization), _capture_cursor_flag(capture_cursor)
{
    /* Fill bitmap information */
    BITMAPINFO _bitmap_info = { 0 };
    _bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    _bitmap_info.bmiHeader.biWidth = _src_width;
    _bitmap_info.bmiHeader.biHeight = -(_src_height);/* Make top-down DIB (upper-left corner) */
    _bitmap_info.bmiHeader.biBitCount = 32u;
    _bitmap_info.bmiHeader.biSizeImage = _src_width * _src_height * 4;
    _bitmap_info.bmiHeader.biPlanes = 1u;
    _bitmap_info.bmiHeader.biCompression = BI_RGB;
    _bitmap_info.bmiHeader.biClrUsed = 0u;
    _bitmap_info.bmiHeader.biClrImportant = 0u;
    _bitmap_info.bmiHeader.biXPelsPerMeter = 0;
    _bitmap_info.bmiHeader.biYPelsPerMeter = 0;

    /* Allocate bitmaps */
    for (uint8_t i = 0u; i < 2u; ++i)
    {
        /* Create bitmap */
        _bitmaps_ctxs[i] = CreateCompatibleDC(nullptr);/* Set arg to null like in OBS code */

        //_bitmap = CreateCompatibleBitmap(_wnd_dev_ctx, _src_width, _src_height);
        //GetDIBits()

        _bitmaps[i] = CreateDIBSection(_src_ctx, (const BITMAPINFO *)&_bitmap_info, DIB_RGB_COLORS, (void **)&_dib_buffers[i], nullptr, 0);
        if (_bitmaps[i] == nullptr) {
            throw std::string("Couldn't allocate frame buffer!");
        }

        _old_objs[i] = SelectObject(_bitmaps_ctxs[i], (HGDIOBJ)_bitmaps[i]);
    }
}

BitmapsDblBuff::~BitmapsDblBuff()
{
    /* Free bitmaps */
    for (uint8_t i = 0u; i < 2u; ++i)
    {
        SelectObject(_bitmaps_ctxs[i], _old_objs[i]);
        DeleteDC(_bitmaps_ctxs[i]);
        DeleteObject((HGDIOBJ)_bitmaps[i]);/* Also release memory for buffer */

        //_dib_buffers[i] = nullptr;/* GDI will release bitmap buffer */
    }
}

HDC BitmapsDblBuff::GetBitmapContext()
{
    return _bitmaps_ctxs[_lock_index];
}

const int &BitmapsDblBuff::GetSrcWidth()
{
    return _src_width;
}

const int &BitmapsDblBuff::GetSrcHeight()
{
    return _src_height;
}
