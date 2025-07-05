#include "Window.h"

#include <regex>
#include <thread>
#include <chrono>

Window::Window(const char *name, const std::pair<int, int> &pos, const std::pair<int, int> &size, const int &icon_id, const unsigned long &wnd_style, WNDPROC wnd_proc)
    : WndBase(pos, size)
{
    /* Add to global list of windows */
    _wnd_index = _wnd_list.Append(&_wnd_pos_size);

    // Register window class
    _w_class_name = std::unique_ptr<wchar_t[]>(to_utf16(std::string(std::regex_replace(name, std::regex(" "), "_") + std::string("_") + std::to_string(_wnd_index) + std::string("_class")).c_str()));

    WNDCLASSW wc = { 0 };
    {
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC /*|  CS_PARENTDC (or this or OWNDC or CLASSDC or none) */;
        wc.lpfnWndProc = wnd_proc ? wnd_proc : Window::_DefWndProc;
        wc.cbClsExtra = 0;// 1 - Reserve for ability change WndProc
        wc.cbWndExtra = sizeof(LONG_PTR);
        wc.hInstance = HINST_THISCOMPONENT;
        wc.hIcon = icon_id ? LoadIconW(HINST_THISCOMPONENT, MAKEINTRESOURCEW(icon_id)) : LoadIconW(NULL, (LPWSTR)IDI_APPLICATION);
        wc.hCursor = LoadCursorW(NULL, (LPWSTR)IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);//NULL
        wc.lpszMenuName = NULL;
        wc.lpszClassName = _w_class_name.get();
    }

    if (RegisterClassW(&wc) == 0)
    {
        _wnd_list.Delete(_wnd_index);
        throw std::string("Can't register class -> Error code: " + std::to_string(GetLastError()));
    }
    
    // Create window
    std::unique_ptr<wchar_t[]> w_name(to_utf16(name));

    _hwnd = CreateWindowExW(
        0L, wc.lpszClassName, w_name.get(), wnd_style,
        _pos.first, _pos.second, _size.first, _size.second,
        NULL, NULL, NULL, NULL);

    if (_hwnd == nullptr)
    {
        _wnd_list.Delete(_wnd_index);
        throw std::string("Can't create window -> Error code: " + std::to_string(GetLastError()));
    }
}

Window::~Window()
{
    if (_wnd_list[_wnd_index])
    {
        DestroyWindow(_hwnd);
    }

    if (UnregisterClassW(_w_class_name.get(), HINST_THISCOMPONENT) == 0)
    {
        MessageBoxW(NULL, std::wstring(L"Can't unregister class -> Error code: " + std::to_wstring(GetLastError())).c_str(), L"Error", MB_OK);
    }
}

void Window::AttachChildControl(WndBase *wnd)
{
    if (dynamic_cast<Button *>(wnd))
    {
        _btns_list.Append(dynamic_cast<Button *>(wnd));
        wnd->SetWndParent(this);
    }
    else if (dynamic_cast<ComboBox *>(wnd))
    {
        _cmb_bxs_list.Append(dynamic_cast<ComboBox *>(wnd));
        wnd->SetWndParent(this);
    }
    else if (dynamic_cast<Edit *>(wnd))
    {
        _edits_list.Append(dynamic_cast<Edit *>(wnd));
        wnd->SetWndParent(this);
    }
    else if (dynamic_cast<Label *>(wnd))
    {
        _lbls_list.Append(dynamic_cast<Label *>(wnd));
        wnd->SetWndParent(this);
    }
    else
    {
        throw std::string("Couldn't attach control to window!");
    }
}

void Window::AttachMenu(Menu *menu)
{
    _menu = menu;
    menu->SetHParent((void *)_hwnd);
}

WndList<Button> &Window::GetButtonsList()
{
    return _btns_list;
}

WndList<ComboBox> &Window::GetComboBoxesList()
{
    return _cmb_bxs_list;
}

WndList<Edit> &Window::GetEditsList()
{
    return _edits_list;
}

WndList<Label> &Window::GetLabelsList()
{
    return _lbls_list;
}

Menu &Window::GetMenu()
{
    return *_menu;
}

void Window::EnableControlsDialogMessages(const bool &flag)
{
    _ctrls_dlg_msg_flag = flag;
}

int Window::Run()
{
    // Program main loop
    if (HasCallback("AsyncLoopCallback"))
    {
        while (true)
        {
            if (PeekMessageW(&_msg, NULL, 0, 0, PM_REMOVE))
            {
                if (_msg.message == WM_QUIT)
                {
                    if (_wnd_list.GetCount() > 0) continue;
                    break;
                }
                else if (_ctrls_dlg_msg_flag)
                {
                    if (!IsDialogMessageW(_hwnd, &_msg))/* -- VK_KEY's messages won't be delivered to main window -- */
                    {
                        TranslateMessage(&_msg);
                        DispatchMessageW(&_msg);
                    }
                }
                else
                {
                    TranslateMessage(&_msg);
                    DispatchMessageW(&_msg);
                }
            }
            else
            {
                this->operator()("AsyncLoopCallback", this);
            }

            std::this_thread::sleep_for(std::chrono::nanoseconds(500));
        }
    }
    else
    {
        while (true)
        {
            if (!GetMessageW(&_msg, NULL, 0, 0))
            {
                if (_wnd_list.GetCount() > 0) continue;
                break;
            }

            if (_ctrls_dlg_msg_flag)
            {
                if (!IsDialogMessageW(_hwnd, &_msg))/* -- VK_KEY's messages won't be delivered to main window -- */
                {
                    TranslateMessage(&_msg);
                    DispatchMessageW(&_msg);
                }
            }
            else
            {
                TranslateMessage(&_msg);
                DispatchMessageW(&_msg);
            }

            std::this_thread::sleep_for(std::chrono::nanoseconds(500));
        }
    }

    return static_cast<int>(_msg.wParam);
}
