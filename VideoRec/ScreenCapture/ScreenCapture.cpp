#include "ScreenCapture.h"

#include <string>

ScreenCapture::ScreenCapture(const char *window_name, const int &dst_width, const int &dst_height)
    : _dst_width(dst_width), _dst_height(dst_height)
{
    /* Get window */
    if (window_name)
    {
        int text_size = (int)strlen(window_name) + 1;
        wchar_t *w_window_name = new wchar_t[text_size] { 0 };
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
    GetClientRect(_hwnd, &rect);
    _src_width = rect.right;
    _src_height = rect.bottom;

    /* Get window device context */
    _wnd_dev_ctx = GetDC(_hwnd);

    /* Fill bitmap information */
    {
        _bitmap_info.biSize = sizeof(BITMAPINFOHEADER);
        //_bitmap_info.biWidth = _src_width;
        _bitmap_info.biWidth = _dst_width;
        //_bitmap_info.biHeight = -(_src_height);/* Make top-down DIB (upper-left corner) */
        _bitmap_info.biHeight = -(_dst_height);/* Make top-down DIB (upper-left corner) */
        _bitmap_info.biBitCount = 32u;
        _bitmap_info.biSizeImage = _dst_width * _dst_height * 4;
        _bitmap_info.biPlanes = 1u;
        _bitmap_info.biCompression = BI_RGB;
        _bitmap_info.biClrUsed = 0u;
        _bitmap_info.biClrImportant = 0u;
        _bitmap_info.biXPelsPerMeter = 0;
        _bitmap_info.biYPelsPerMeter = 0;
    }

    /* Create frame buffer */
    _buffer = new uint8_t[_dst_width * _dst_height * 4] { 0 };

    _bitmap_ctx = CreateCompatibleDC(_wnd_dev_ctx);
    //_bitmap = CreateCompatibleBitmap(_wnd_dev_ctx, _width, _height);
    if ((_bitmap = CreateDIBSection(_wnd_dev_ctx, (const BITMAPINFO *)&_bitmap_info, DIB_RGB_COLORS, (void **)&_buffer, nullptr, 0)) == NULL) {
        throw std::string("Couldn't allocate frame buffer!");
    }

    _old_obj = SelectObject(_bitmap_ctx, (HGDIOBJ)_bitmap);
}

ScreenCapture::~ScreenCapture()
{
    _buffer = nullptr;/* GDI will release bitmap buffer */

    /* Release resources */
    //GdiFlush();/* Flushes the calling thread's current batch *//* ??? */
    SelectObject(_bitmap_ctx, _old_obj);
    DeleteDC(_bitmap_ctx);
    DeleteObject((HGDIOBJ)_bitmap);
    ReleaseDC(_hwnd, _wnd_dev_ctx);
}

void ScreenCapture::TakeShot()
{
    StretchBlt(_bitmap_ctx, 0, 0, _dst_width, _dst_height, _wnd_dev_ctx, 0, 0, _src_width, _src_height, SRCCOPY);
}

int ScreenCapture::GetDstWidth()
{
    return _dst_width;
}

int ScreenCapture::GetDstHeight()
{
    return _dst_height;
}

uint8_t *ScreenCapture::GetScreenShotBuffer()
{
    return _buffer;
}
