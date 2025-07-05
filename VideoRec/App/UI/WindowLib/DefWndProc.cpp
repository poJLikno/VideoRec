#include "Window.h"

WndList<WindowPosSize> Window::_wnd_list;

template<class T>
static void ParentResizeCallbacksCaller(WndList<T> &ctrls_list, ParentResizeCallbackParams *params);

template<class T>
static void MainCallbacksCaller(WndList<T> &ctrls_list, LPARAM &lparam);

static bool MenuMainCallbacksCaller(MenuBase *menu, WPARAM &wparam);

LRESULT Window::_DefWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;

    try
    {
        // Check windows
        Window *Wnd = nullptr;
        std::pair<int, int> *WndPos = nullptr;
        std::pair<int, int> *WndSize = nullptr;

        for (int i = 0; i < _wnd_list.GetListLength(); ++i)
        {
            if (_wnd_list[i] && hwnd == _wnd_list[i]->wnd->GetHwnd())
            {
                Wnd = _wnd_list[i]->wnd;
                WndPos = _wnd_list[i]->pos;
                WndSize = _wnd_list[i]->size;
                break;
            }
        }

        if (Wnd == nullptr)
        {
            return DefWindowProcW(hwnd, msg, wparam, lparam);
        }

        /* Start message check */
        if (msg == WM_CREATE)
        {
            result = 0;
        }
        /*if (msg == WM_KEYDOWN)
        {
            if (wparam == VK_ESCAPE)
            {
                DestroyWindow(hwnd);
            }

            result = 0;
        }*/
        //else if (msg == WM_GETMINMAXINFO)
        //{
        //    LPMINMAXINFO lpMMI = (LPMINMAXINFO)lparam;
        //    lpMMI->ptMinTrackSize.x = 520/* client */ + (2 * Wnd->GetWndSideBorderSize());
        //    lpMMI->ptMinTrackSize.y = 77/* client */ + Wnd->GetWndHeaderSize() + Wnd->GetWndBottomBorderSize();
        //
        //    result = 0;
        //}
        else if (msg == WM_CTLCOLORSTATIC)/* !!!For transparent "Label" !!!*/
        {
            SetBkMode((HDC)wparam, TRANSPARENT);
            result = 0;/* must be a brush handle | ??? */
        }
        /*else if (msg == WM_MOUSEMOVE)
        {
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);

            std::cout << x << " : " << y << "\n";

            result = 0;
        }*/
        else if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN)
        {
            SetFocus(Wnd->GetHwnd());

            result = 0;
        }
        else if (msg == WM_MOVE)
        {
            /* Update window pos */
            Wnd->GetWndPos();

            result = 0;
        }
        else if (msg == WM_SIZE)
        {
            ParentResizeCallbackParams params = { nullptr, *WndSize };

            /* Update window size */
            Wnd->GetWndSize();

            // Run parent resize controls callbacks
            ParentResizeCallbacksCaller(Wnd->GetButtonsList(), &params);
            ParentResizeCallbacksCaller(Wnd->GetComboBoxesList(), &params);
            ParentResizeCallbacksCaller(Wnd->GetEditsList(), &params);
            ParentResizeCallbacksCaller(Wnd->GetLabelsList(), &params);

            UpdateWindow(hwnd);

            result = 0;
        }
        else if (msg == WM_COMMAND)
        {
            // Run main controls callbacks
            if (HIWORD(wparam) == BN_CLICKED)
            {
                MainCallbacksCaller(Wnd->GetButtonsList(), lparam);
            }
            else if (HIWORD(wparam) == CBN_SELCHANGE)
            {
                MainCallbacksCaller(Wnd->GetComboBoxesList(), lparam);
            }
            else if (HIWORD(wparam) == EN_UPDATE)
            {
                MainCallbacksCaller(Wnd->GetEditsList(), lparam);
            }
            //else if (HIWORD(wparam) == EN_CHANGE)/* Also edit message */
            //{
            //    std::wcout << L"EN_CHANGE\r\n";
            //}
            
            // Run menu points main callbacks
            MenuMainCallbacksCaller(Wnd->_menu, wparam);

            result = 0;
        }
        else if (msg == WM_CLOSE)
        {
            DestroyWindow(hwnd);
            result = 0;
        }
        else if (msg == WM_DESTROY)
        {
            /* Remve this window from global lists */
            _wnd_list.Delete(Wnd->_wnd_index);

            PostQuitMessage(0);
            result = 0;
        }
        else
        {
            result = DefWindowProcW(hwnd, msg, wparam, lparam);
        }
    }
    catch (const std::string &error)
    {
        std::unique_ptr<wchar_t[]> w_error(to_utf16(error.c_str()));

        MessageBoxW(NULL, w_error.get(), L"Error", MB_OK);

        DestroyWindow(hwnd);
    }

    return result;
}

template<class T>
static void ParentResizeCallbacksCaller(WndList<T> &ctrls_list, ParentResizeCallbackParams *params)
{
    for (int i = 0; i < ctrls_list.GetListLength(); ++i)
    {
        if (ctrls_list[i])
        {
            params->wnd = ctrls_list[i];
            ctrls_list[i]->operator()("ParentResizeCallback", params);
        }
    }
}

template<class T>
static void MainCallbacksCaller(WndList<T> &ctrls_list, LPARAM &lparam)
{
    for (int i = 0; i < ctrls_list.GetListLength(); ++i)
    {
        if (ctrls_list[i] && lparam == (LPARAM)ctrls_list[i]->GetHwnd())
        {
            ctrls_list[i]->operator()("MainCallback", ctrls_list[i]);
            break;
        }
    }
}

static bool MenuMainCallbacksCaller(MenuBase *menu, WPARAM &wparam)//, bool &ctrl_was_found_flag) {
{
    if (menu == nullptr)
    {
        return false;
    }

    for (int i = 0; i < menu->GetMenuPointsList().GetListLength(); ++i)
    {
        MenuPoint *menu_point = menu->GetMenuPointsList()[i];
        if (menu_point && LOWORD(wparam) == menu_point->GetId())
        {
            menu_point->operator()("MainCallback", menu_point);
            return true;
        }
    }

    for (int i = 0; i < menu->GetPopupMenusList().GetListLength(); ++i)
    {
        MenuBase *popup_menu = menu->GetPopupMenusList()[i];
        if (popup_menu && MenuMainCallbacksCaller(popup_menu, wparam))
        {
            break;/* No difference break, return true/false */
        }
    }

    return false;
}
