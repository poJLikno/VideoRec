#include "PreviewWindow.h"

#include <string>

WndList<PreviewWindow> PreviewWindow::_wnd_list;
WndList<WndPairValue> PreviewWindow::_wnd_pos_list;
WndList<WndPairValue> PreviewWindow::_wnd_size_list;

LRESULT PreviewWindow::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    try
    {
        // Check windows
        PreviewWindow *Wnd = nullptr;
        WndPairValue *WndPos = nullptr;
        WndPairValue *WndSize = nullptr;
        for (int i = 0; i < _wnd_list.GetCount(); ++i) {
            if (hwnd == _wnd_list[i]->GetHwnd()) {
                Wnd = _wnd_list[i];
                WndPos = _wnd_pos_list[i];
                WndSize = _wnd_size_list[i];
                break;
            }
        }
        if (!Wnd) return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        if (uMsg == WM_CREATE) {
            result = 1;
        }
        else {
            if (uMsg == WM_PAINT) {
                /* Get device context */
                PAINTSTRUCT ps = { 0 };
                HDC hdc = BeginPaint(hwnd, &ps);
                /* Configure DC */
                SetStretchBltMode(hdc, HALFTONE);/* Make normal scale */
                HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                HGDIOBJ old_obj = SelectObject(hdc, (HGDIOBJ)pen);
                SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

                /* Paint */
                auto [wnd_width, wnd_height] = Wnd->GetWndSize();
                if (Wnd->_bitmaps_dbl_buff)
                {
                    SmtObj<BitmapsDblBuff> &frames_dbl_buff = *Wnd->_bitmaps_dbl_buff;
                    frames_dbl_buff->Lock();
                    StretchBlt(
                        hdc, 0, 0, wnd_width, wnd_height,
                        frames_dbl_buff->GetBitmapContext(), 0, 0, frames_dbl_buff->GetSrcWidth(), frames_dbl_buff->GetSrcHeight(),
                        SRCCOPY);
                    frames_dbl_buff->Unlock();
                }
                else
                {
                    SelectObject(hdc, GetStockObject(WHITE_BRUSH));
                }
                Rectangle(hdc, 0, 0, wnd_width, wnd_height);

                /**/

                /* Cleanup */
                SelectObject(hdc, old_obj);
                DeleteObject((HGDIOBJ)pen);

                EndPaint(hwnd, &ps);
                result = 0;
            }
            else if (uMsg == WM_CLOSE) {
                DestroyWindow(hwnd);
                result = 0;
            }
            else if (uMsg == WM_DESTROY) {
                PostQuitMessage(0);
                result = 1;
            }
            else result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
        }
    }
    catch (const std::string &error)
    {
        int str_size = (int)error.length() + 1;
        SmtObj<wchar_t[]> w_error = new wchar_t[str_size] { 0 };
        MultiByteToWideChar(CP_UTF8, 0, error.c_str(), str_size, w_error, str_size);

        MessageBoxW(NULL, w_error, L"Error", MB_OK);
        DestroyWindow(hwnd);
        result = 0;
    }

    return result;
}

PreviewWindow::PreviewWindow(WndBase *parent_wnd, const WndPairValue &pos, const WndPairValue &size)
    : WndBase(pos, size)
{
    // Register window class
    WNDCLASSW wc = { 0 };
    {
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = PreviewWindow::WndProc;
        wc.cbClsExtra = 0;// 1 - Reserve for ability change WndProc
        wc.cbWndExtra = sizeof(LONG_PTR);
        wc.hInstance = NULL;
        wc.hIcon = LoadIconW(NULL, (LPWSTR)IDI_APPLICATION);
        wc.hCursor = LoadCursorW(NULL, (LPWSTR)IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);//NULL
        wc.lpszMenuName = NULL;
        wc.lpszClassName = L"preview_window_class";
    }
    if (!RegisterClassW(&wc))
        throw std::string("Can't register class -> Error code: " + std::to_string(GetLastError()));

    // Create window
    this->_hwnd = CreateWindowExW(0L, wc.lpszClassName, L"Preview window", WS_POPUP,
        _pos.first, _pos.second, _size.first, _size.second,
        NULL, NULL, NULL, NULL);
    if (!this->_hwnd)
    {
        throw std::string("Can't create window -> Error code: " + std::to_string(GetLastError()));
    }

    /* Add to global list of windows */
    _wnd_list.Append(this);
    _wnd_pos_list.Append(&_pos);
    _wnd_size_list.Append(&_size);
    _wnd_list_index = _wnd_list.GetCount() - 1;

    WndBase::SetWndParent(parent_wnd);

    /* Create a paint timer loop */
    _paint_timer = new std::thread([this]()->void {
        std::chrono::time_point<std::chrono::steady_clock> millis_timer = std::chrono::steady_clock::now();

        while (_timer_flag)
        {
            /* Draw window ~60 times per second */
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - millis_timer).count() >= 16)
            {
                millis_timer = std::chrono::steady_clock::now();
                InvalidateRect(_hwnd, NULL, FALSE);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        });
}

PreviewWindow::~PreviewWindow()
{
    _timer_flag = false;
    _paint_timer->join();    

    if (!UnregisterClassW(L"preview_window_class", NULL))
        MessageBoxW(NULL, std::wstring(L"Can't unregister class -> Error code: " + std::to_wstring(GetLastError())).c_str(), L"Error", MB_OK);

    /* Remve this window from global lists */
    _wnd_list[_wnd_list_index] = nullptr;
    _wnd_pos_list[_wnd_list_index] = nullptr;
    _wnd_size_list[_wnd_list_index] = nullptr;
}

void PreviewWindow::SetPreview(SmtObj<BitmapsDblBuff> &frames_dbl_buff)
{
    _bitmaps_dbl_buff = &frames_dbl_buff;
}

void PreviewWindow::DeletePreview()
{
    _bitmaps_dbl_buff = nullptr;
}
