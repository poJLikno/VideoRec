#include "ScreenCapture.h"

#include <string>

ScreenCapture::ScreenCapture(const char *window_name, const bool &client_rect_only, const int &dst_width, const int &dst_height)
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

    /* Get window size */
    RECT rect = { 0 };
    POINT upper_left = { 0 };

    /* check iconic (minimized) twice, ABA is very unlikely (OBS code segment) */
    bool window_available = !IsIconic(_hwnd) && GetClientRect(_hwnd, &rect) && !IsIconic(_hwnd) &&
        (rect.right > 0) && (rect.bottom > 0) && ClientToScreen(_hwnd, &upper_left);

    if (!window_available)
    {
        /* Make window opened */
        ShowWindow(_hwnd, SW_RESTORE);
        GetClientRect(_hwnd, &rect);
    }
    int dpi = GetDpiForWindow(_hwnd);
    _src_width = (rect.right - rect.left) * dpi / USER_DEFAULT_SCREEN_DPI;
    _src_height = (rect.bottom - rect.top) * dpi / USER_DEFAULT_SCREEN_DPI;

    _dst_width = (dst_width == -1 ? _src_width : dst_width);
    _dst_height = (dst_height == -1 ? _src_height : dst_height);

    /* Get window device context */
    _wnd_dev_ctx = GetDCEx(_hwnd, nullptr, (client_rect_only ? 0 : DCX_WINDOW) | DCX_CACHE | DCX_LOCKWINDOWUPDATE);
    if (!_wnd_dev_ctx)
    {
        throw std::string("Couldn't get window's DC!");
    }

    /* Fill bitmap information */
    {
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
    }

    /* Create bitmap */
    _bitmap_ctx = CreateCompatibleDC(_wnd_dev_ctx);

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
    GdiFlush();/* Flushes the calling thread's current batch *//* ??? */

    SelectObject(_bitmap_ctx, _old_obj);
    DeleteDC(_bitmap_ctx);
    DeleteObject((HGDIOBJ)_bitmap);/* Also release memory for buffer */
    ReleaseDC(_hwnd, _wnd_dev_ctx);

    _buffer = nullptr;/* GDI will release bitmap buffer */
}

void ScreenCapture::TakeShot()
{
    BitBlt(_bitmap_ctx, 0, 0, _src_width, _src_height, _wnd_dev_ctx, 0, 0, SRCCOPY);
    GdiFlush();/* Flushes the calling thread's current batch *//* ??? */
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
