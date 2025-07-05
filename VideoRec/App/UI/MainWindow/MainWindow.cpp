#include "MainWindow.h"

Window *MainWindow::_MainWnd = nullptr;

LRESULT MainWindow::_MainWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (_MainWnd == nullptr)
    {
        return _DefWndProc(hwnd, msg, wparam, lparam);
    }

    LRESULT result = 0;
    static int a = 0;

    if (msg == WM_GETMINMAXINFO)
    {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lparam;
        lpMMI->ptMinTrackSize.x = 520/*client*/ + (2 * 8/*defailt size of side border*/);
        lpMMI->ptMinTrackSize.y = 77/*client*/ + (31/*default size of window header*/ + 20/*menu height*/) + 8/*default size of bottom border*/;

        result = 0;
    }
    else
    {
        result = _DefWndProc(hwnd, msg, wparam, lparam);
    }

    return result;
}

MainWindow::MainWindow(const char *name, const std::pair<int, int> &pos, const std::pair<int, int> &size, const int &icon_id, const unsigned long &wnd_style)
    : Window(name, pos, size, icon_id, wnd_style, MainWindow::_MainWndProc)
{
    _MainWnd = _wnd_list[_wnd_index]->wnd;
}

MainWindow::~MainWindow()
{
    _MainWnd = nullptr;
}
