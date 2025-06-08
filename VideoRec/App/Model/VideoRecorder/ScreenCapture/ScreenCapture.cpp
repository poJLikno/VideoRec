#include "ScreenCapture.h"

#include <string>

ScreenCapture::ScreenCapture(const char *window_name, const bool &client_rect_only, const bool &use_optimization, const bool &capture_cursor, const int &dst_width, const int &dst_height)
    : _window_name(window_name)
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
#pragma warning(push)/* Suppress this warning only here */
#pragma warning(disable : 6387)
    int dpi = (window_name ? GetDpiForWindow(_hwnd) : GetSystemDpiForProcess(NULL));
//#pragma warning(default : 6387) set to default (enable)
#pragma warning(pop)

    /* Get source frame resolution */
    _src_width = (rect.right - rect.left) * dpi / USER_DEFAULT_SCREEN_DPI;
    _src_height = (rect.bottom - rect.top) * dpi / USER_DEFAULT_SCREEN_DPI;

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
        _cursors_dbl_buff = new CursorsDblBuff(_hwnd, (_window_name != nullptr), client_rect_only, dpi);
    }

    /* Create bitmaps double buffer */
    _bitmaps_dbl_buff = new BitmapsDblBuff(
        _hwnd, _wnd_dev_ctx,
        _src_width, _src_height,
        _frames_dbl_buff,
        _cursors_dbl_buff,
        use_optimization, capture_cursor);
}

ScreenCapture::~ScreenCapture()
{
    /* Release resources */
    _bitmaps_dbl_buff.reset();
    _frames_dbl_buff.reset();
    _cursors_dbl_buff.reset();

    ReleaseDC(_hwnd, _wnd_dev_ctx);
}

void ScreenCapture::CaptureScreenFrame()
{
    _bitmaps_dbl_buff->Write();
}

void ScreenCapture::CaptureCursorState()
{
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

/* Another way to get monitor DPI */
//int dpi = 96;

//if (window_name == nullptr)/*)
//{
//    dpi = GetDpiForWindow(_hwnd);
//}
//else*/
//{
//    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR monitor, HDC monitor_hdc, LPRECT rect, LPARAM lParam)->BOOL {
//        if (monitor == nullptr)
//        {
//            throw std::string("Couldn't get monitor!");
//        }

//        MONITORINFOEX monitor_info = { 0 };
//        monitor_info.cbSize = sizeof(MONITORINFOEX);
//        if (GetMonitorInfoW(monitor, &monitor_info) == 0)
//        {
//            throw std::string("Couldn't get monitor info!");
//        }

//        if ((monitor_info.dwFlags & MONITORINFOF_PRIMARY) == 0)
//        {
//            return TRUE;
//        }

//        DEVMODE dev_mode = { 0 };
//        dev_mode.dmSize = sizeof(DEVMODE);
//        if (EnumDisplaySettingsW(monitor_info.szDevice, ENUM_CURRENT_SETTINGS, &dev_mode) == 0)
//        {
//            throw std::string("Couldn't get current display settings!");
//        }

//        int &l_dpi = *((int *)lParam);
//        l_dpi = /*real_width*/dev_mode.dmPelsWidth * USER_DEFAULT_SCREEN_DPI / /*virtual_width*/(monitor_info.rcMonitor.right - monitor_info.rcMonitor.left);

//        return TRUE;

//        }, (LPARAM)&dpi);
//}
