#include "ScreenCapture.h"

#include <string>

void ScreenCapture::_DrawCursor()
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

        DrawIconEx(
            _bitmap_ctx,/* dst */
            (cursor_info.ptScreenPos.x - (int)icon_info.xHotspot - rect.left) * _dpi / USER_DEFAULT_SCREEN_DPI,/* x */
            (cursor_info.ptScreenPos.y - (int)icon_info.yHotspot - rect.top) * _dpi / USER_DEFAULT_SCREEN_DPI,/* y */
            cursor_info.hCursor, bmp.bmWidth * _dpi / USER_DEFAULT_SCREEN_DPI,/* width */
            bmp.bmHeight * _dpi / USER_DEFAULT_SCREEN_DPI,/* height */
            0, NULL, DI_NORMAL);

        DeleteObject(icon_info.hbmColor);
        DeleteObject(icon_info.hbmMask);
    }
}

ScreenCapture::ScreenCapture(const char *window_name, const bool &client_rect_only, bool &use_optimization, bool &capture_cursor, const int &dst_width, const int &dst_height)
    : _client_rect_only_flag(client_rect_only), _use_optimization_flag(use_optimization), _capture_cursor_flag(capture_cursor)
{
    /* Get window */
    if (window_name)
    {
        int text_size = (int)strlen(window_name) + 1;
        SmtObj<wchar_t[]> w_window_name = new wchar_t[text_size] { 0 };
        MultiByteToWideChar(CP_UTF8, 0, window_name, text_size, w_window_name, text_size);

        _hwnd = FindWindowW(NULL, w_window_name);
        if (_hwnd == NULL)
        {
            throw std::string("Couldn't find window!");
        }
    }
    else
    {
        _hwnd = GetDesktopWindow();
    }

    /* Get window device context */
    _wnd_dev_ctx = GetDCEx(_hwnd, nullptr, (client_rect_only ? 0 : DCX_WINDOW) | DCX_CACHE | DCX_LOCKWINDOWUPDATE);
    if (!_wnd_dev_ctx)
    {
        throw std::string("Couldn't get window's DC!");
    }

    /* Get window size */
    RECT rect = { 0 };
    POINT upper_left = { 0 };

    /* check iconic (minimized) twice, ABA is very unlikely (OBS code segment) */
    bool window_available =
        !IsIconic(_hwnd)
        && (client_rect_only ? GetClientRect(_hwnd, &rect) : GetWindowRect(_hwnd, &rect))
        && (rect.right > 0) && (rect.bottom > 0)
        && (client_rect_only ? ClientToScreen(_hwnd, &upper_left) : 1);

    if (!window_available)
    {
        /* Make window opened */
        ShowWindow(_hwnd, SW_RESTORE);

        /* Prepare structures */
        memset(&rect, 0, sizeof(RECT));

        /* Get size */
        if (client_rect_only)
        {
            GetClientRect(_hwnd, &rect);
        }
        else
        {
            GetWindowRect(_hwnd, &rect);
        }
    }

    /* Get screen DPI */
    _dpi = (window_name ? GetDpiForWindow(_hwnd) : GetSystemDpiForProcess(NULL));

    /* Get source frame resolution */
    _src_width = (rect.right - rect.left) * _dpi / USER_DEFAULT_SCREEN_DPI;
    _src_height = (rect.bottom - rect.top) * _dpi / USER_DEFAULT_SCREEN_DPI;

    /* Set destination frame resolution */
    _dst_width = (dst_width == -1 ? _src_width : dst_width);
    _dst_height = (dst_height == -1 ? _src_height : dst_height);

    /* Fill bitmap information */
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

    /* Create bitmap */
    _bitmap_ctx = CreateCompatibleDC(nullptr/*_wnd_dev_ctx*/);/* Set arg to null like in OBS code */

    //_bitmap = CreateCompatibleBitmap(_wnd_dev_ctx, _src_width, _src_height);
    //GetDIBits()

    _bitmap = CreateDIBSection(_wnd_dev_ctx, (const BITMAPINFO *)&_bitmap_info, DIB_RGB_COLORS, (void **)&_buffer, nullptr, 0);
    if (_bitmap == nullptr) {
        throw std::string("Couldn't allocate frame buffer!");
    }

    _old_obj = SelectObject(_bitmap_ctx, (HGDIOBJ)_bitmap);

    /* Create double buffer */
    _frames_buffer = new DoubleBuffer(_buffer, _src_width, _src_height, _dst_width, _dst_height);
}

ScreenCapture::~ScreenCapture()
{
    /* Release resources */
    //GdiFlush();/* Flushes the calling thread's current batch *//* ??? */

    SelectObject(_bitmap_ctx, _old_obj);
    DeleteDC(_bitmap_ctx);
    DeleteObject((HGDIOBJ)_bitmap);/* Also release memory for buffer */
    ReleaseDC(_hwnd, _wnd_dev_ctx);

    _buffer = nullptr;/* GDI will release bitmap buffer */
}

void ScreenCapture::TakeShot()
{
    if (_use_optimization_flag)
    {
        BitBlt(_bitmap_ctx, 0, 0, _src_width, _src_height, _wnd_dev_ctx, 0, 0, SRCCOPY);
    }
    else
    {
        PrintWindow(_hwnd, _bitmap_ctx, PW_RENDERFULLCONTENT);
    }

    /* Draw a cursor */
    if (_capture_cursor_flag)
    {
        _DrawCursor();
    }

    _frames_buffer->WriteFrame();
}

const int &ScreenCapture::GetSrcWidth()
{
    return _src_width;
}

const int &ScreenCapture::GetSrcHeight()
{
    return _src_height;
}

const int &ScreenCapture::GetDstWidth()
{
    return _dst_width;
}

const int &ScreenCapture::GetDstHeight()
{
    return _dst_height;
}

DoubleBuffer *ScreenCapture::GetFramesBuffer()
{
    return _frames_buffer;
}

HDC ScreenCapture::GetBitmapContextForPreview()
{
    return _bitmap_ctx;
}
