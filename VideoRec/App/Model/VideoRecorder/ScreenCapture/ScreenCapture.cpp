#include "ScreenCapture.h"

#include <string>

ScreenCapture::ScreenCapture(const char *window_name, const bool &client_rect_only, const bool &use_optimization, const bool &capture_cursor, const int &dst_width, const int &dst_height)
    : _window_name(window_name), _client_rect_only_flag(client_rect_only)
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
    _wnd_dev_ctx = GetDCEx(_hwnd, nullptr, (_client_rect_only_flag ? 0 : DCX_WINDOW) | DCX_CACHE | DCX_LOCKWINDOWUPDATE);
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
        && (_client_rect_only_flag ? GetClientRect(_hwnd, &rect) : GetWindowRect(_hwnd, &rect))
        && (rect.right > 0) && (rect.bottom > 0)
        && (_client_rect_only_flag ? ClientToScreen(_hwnd, &upper_left) : 1);

    if (!window_available)
    {
        /* Make window opened */
        ShowWindow(_hwnd, SW_RESTORE);

        /* Prepare structures */
        memset(&rect, 0, sizeof(RECT));

        /* Get size */
        if (_client_rect_only_flag)
        {
            GetClientRect(_hwnd, &rect);
        }
        else
        {
            GetWindowRect(_hwnd, &rect);
        }
    }

    /* Get screen DPI */
#pragma warning(push)/* Suppress this warning only here */
#pragma warning(disable : 6387)
    _dpi = (window_name ? GetDpiForWindow(_hwnd) : GetSystemDpiForProcess(NULL));
//#pragma warning(default : 6387) set to default (enable)
#pragma warning(pop)

    /* Get source frame resolution */
    _src_width = (rect.right - rect.left) * _dpi / USER_DEFAULT_SCREEN_DPI;
    _src_height = (rect.bottom - rect.top) * _dpi / USER_DEFAULT_SCREEN_DPI;

    /* Set destination frame resolution */
    _dst_width = (dst_width == -1 ? _src_width : dst_width);
    _dst_height = (dst_height == -1 ? _src_height : dst_height);


    /* Create frames double buffer */
    _frames_dbl_buff = new FramesDblBuff(
        _src_width, _src_height,
        _dst_width, _dst_height);

    /* Create cursors double buffer */
    if (capture_cursor)
    {
        _cursors_dbl_buff = new CursorsDblBuff();
    }

    /* Create bitmaps double buffer */
    _bitmaps_dbl_buff = new BitmapsDblBuff(
        _hwnd, _wnd_dev_ctx,
        _src_width, _src_height,
        _dst_width, _dst_height,
        _frames_dbl_buff,
        _cursors_dbl_buff,
        use_optimization, capture_cursor);
}

ScreenCapture::~ScreenCapture()
{
    /* Release resources */
    //GdiFlush();/* Flushes the calling thread's current batch *//* ??? */

    _bitmaps_dbl_buff.reset();
    _frames_dbl_buff.reset();
    _cursors_dbl_buff.reset();

    ReleaseDC(_hwnd, _wnd_dev_ctx);
}

void ScreenCapture::TakeShot()
{
    _bitmaps_dbl_buff->Write();
}

void ScreenCapture::CaptureCursorState()
{
    Cursor cursor = { 0 };

    if (!_cursors_dbl_buff)
    {
        throw std::string("Cursors double buffer is null!");
    }
    else if (_window_name ? GetForegroundWindow() == _hwnd : 1)
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

            
            cursor.h_cursor = cursor_info.hCursor;
            cursor.relative_pos_x = (cursor_info.ptScreenPos.x - (int)icon_info.xHotspot - rect.left) * _dpi / USER_DEFAULT_SCREEN_DPI;
            cursor.relative_pos_y = (cursor_info.ptScreenPos.y - (int)icon_info.yHotspot - rect.top) * _dpi / USER_DEFAULT_SCREEN_DPI;
            cursor.width = bmp.bmWidth * _dpi / USER_DEFAULT_SCREEN_DPI;
            cursor.height = bmp.bmHeight * _dpi / USER_DEFAULT_SCREEN_DPI;

            DeleteObject(icon_info.hbmColor);
            DeleteObject(icon_info.hbmMask);
        }        
    }

    _cursors_dbl_buff->SetCursor(cursor);
    _cursors_dbl_buff->Write();
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

SmtObj<BitmapsDblBuff> &ScreenCapture::GetBitmapsDblBuff()
{
    return _bitmaps_dbl_buff;
}

SmtObj<FramesDblBuff> &ScreenCapture::GetFramesDblBuff()
{
    return _frames_dbl_buff;
}
