#include "WndBase.h"

#include <memory>

WndBase::~WndBase()
{
    _parent_wnd = nullptr;
}

void WndBase::ShowWnd(const bool &flag)
{
    _is_visible = flag;
    /*ShowWindow(_hwnd, (flag ? SW_SHOWNORMAL : SW_HIDE));*/
    ShowWindow(_hwnd, (_is_visible ? SW_RESTORE : SW_HIDE));
    if (_is_visible) UpdateWnd();
}

LRESULT WndBase::SendMsg(const UINT &msg, const WPARAM &wparam, const LPARAM &lparam)
{
    return SendMessageW(_hwnd, msg, wparam, lparam);
}

HWND WndBase::GetHwnd()
{
    return _hwnd;
}

bool WndBase::GetInputState() const
{
    return _input_state;
}

void WndBase::SetInputState(const bool &flag)
{
    _input_state = flag;
    EnableWindow(_hwnd, _input_state);
}

/* Need free after use (delete[] ...); max_symbols_count include null-symbol */
char *WndBase::GetWndText(const int &max_symbols_count)
{
    std::unique_ptr<wchar_t[]> w_buffer = std::make_unique<wchar_t[]>(max_symbols_count);

    if (GetWindowTextW(_hwnd, w_buffer.get(), max_symbols_count) == 0)
    {
        int error_code = GetLastError();

        if (error_code != 0)
        {
            throw std::string("Can't get text -> Error code: " + std::to_string(error_code));
        }
    }
    
    return to_utf8(w_buffer.get());
}

void WndBase::SetWndText(const char *text)
{
    std::unique_ptr<wchar_t[]>w_buffer(to_utf16(text));

    if (SetWindowTextW(_hwnd, w_buffer.get()) == 0)
    {
        throw std::string("Can't set text -> Error code: " + std::to_string(GetLastError()));
    }
}

LONG_PTR WndBase::GetWndStyle()
{
    return GetWindowLongPtrW(_hwnd, GWL_STYLE);
}

void WndBase::SetWndStyle(const LONG_PTR &params)
{
    if (SetWindowLongPtrW(_hwnd, GWL_STYLE, params) == 0)
    {
        throw std::string("Can't set window style -> Error code: " + std::to_string(GetLastError()));
    }

    SetWindowPos(_hwnd, NULL, 0, 0, 0, 0, (_is_visible ? SWP_SHOWWINDOW : 0) | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

    UpdateWnd();
}

WndBase *WndBase::GetWndParent()
{
    return _parent_wnd;
}

void WndBase::SetWndParent(WndBase *wnd)
{
    if (wnd)
    {
        LONG_PTR old_style = GetWindowLongPtrW(_hwnd, GWL_STYLE);
        SetWindowLongPtrW(_hwnd, GWL_STYLE, (old_style & ~WS_POPUP) | WS_CHILD);
        SetWindowPos(_hwnd, NULL, 0, 0, 0, 0, (_is_visible ? SWP_SHOWWINDOW : 0) | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
    }
    else
    {
        LONG_PTR old_style = GetWindowLongPtrW(_hwnd, GWL_STYLE);
        SetWindowLongPtrW(_hwnd, GWL_STYLE, (old_style & ~WS_CHILD) | WS_POPUP);
        SetWindowPos(_hwnd, NULL, 0, 0, 0, 0, (_is_visible ? SWP_SHOWWINDOW : 0) | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
    }
        
    if (SetParent(_hwnd, wnd->GetHwnd()) == 0)
    {
        throw std::string("Can't set parent window -> Error code: " + std::to_string(GetLastError()));
    }

    _parent_wnd = wnd;
    UpdateWnd();
}

void WndBase::UpdateWnd()
{
    UpdateWindow(_hwnd);
}

std::pair<int, int> WndBase::GetWndPos()
{
    RECT rect = { 0 };
    GetWindowRect(_hwnd, &rect);

    _pos = { rect.left, rect.top };

    if (_parent_wnd)
    {
        POINT parent_point = { 0, 0 };
        ClientToScreen(_parent_wnd->_hwnd, &parent_point);

        _pos.first -= parent_point.x;
        _pos.second -= parent_point.y;
    }

    return _pos;
}

void WndBase::SetWndPos(const std::pair<int, int> &pos)
{
    if (SetWindowPos(_hwnd, NULL, pos.first, pos.second, 0, 0, (_is_visible ? SWP_SHOWWINDOW : 0) | SWP_NOSIZE | SWP_NOZORDER) == 0)
    {
        throw std::string("Can't set window position -> Error code: " + std::to_string(GetLastError()));
    }
    
    _pos = pos;
    UpdateWnd();
}

std::pair<int, int> WndBase::GetWndSize()
{
    RECT rect = { 0 };
    GetWindowRect(_hwnd, &rect);

    _size = { rect.right - rect.left, rect.bottom - rect.top };

    return _size;
}

void WndBase::SetWndSize(const std::pair<int, int> &size)
{
    if (SetWindowPos(_hwnd, NULL, 0, 0, size.first, size.second, (_is_visible ? SWP_SHOWWINDOW : 0) | SWP_NOMOVE | SWP_NOZORDER) == 0)
    {
        throw std::string("Can't set window size -> Error code: " + std::to_string(GetLastError()));
    }
    
    _size = size;
    UpdateWnd();
}

int WndBase::GetWndHeaderSize()
{
    RECT wnd_rect = { 0, 0 };
    GetWindowRect(_hwnd, &wnd_rect);

    POINT client_point = { 0, 0 };
    ClientToScreen(_hwnd, &client_point);

    return client_point.y - wnd_rect.top;
}

int WndBase::GetWndSideBorderSize()
{
    auto [wnd_width, wnd_height] = GetWndSize();

    RECT client_rect = { 0 };
    GetClientRect(_hwnd, &client_rect);

    return (wnd_width - client_rect.right) / 2;
}

int WndBase::GetWndBottomBorderSize()
{
    auto [wnd_width, wnd_height] = GetWndSize();

    RECT client_rect = { 0 };
    GetClientRect(_hwnd, &client_rect);

    return wnd_height - GetWndHeaderSize() - client_rect.bottom;
}

std::pair<int, int> WndBase::GetWndClientPos()
{
    POINT client_point = { 0, 0 };
    ClientToScreen(_hwnd, &client_point);

    return { client_point.x, client_point.y };
}

void WndBase::SetWndClientPos(const std::pair<int, int> &pos)
{
    std::pair<int, int> wnd_pos = { pos.first - GetWndSideBorderSize(), pos.second - GetWndHeaderSize() };

    if (SetWindowPos(_hwnd, NULL, wnd_pos.first, wnd_pos.second, 0, 0, (_is_visible ? SWP_SHOWWINDOW : 0) | SWP_NOSIZE | SWP_NOZORDER) == 0)
    {
        throw std::string("Can't set window position -> Error code: " + std::to_string(GetLastError()));
    }

    _pos = std::move(wnd_pos);
    UpdateWnd();
}

std::pair<int, int> WndBase::GetWndClientSize()
{
    RECT client_rect = { 0 };
    GetClientRect(_hwnd, &client_rect);

    return { client_rect.right, client_rect.bottom };
}

void WndBase::SetWndClientSize(const std::pair<int, int> &size)
{
    std::pair<int, int> wnd_size = { size.first + (2 * GetWndSideBorderSize()), size.second + GetWndHeaderSize() + GetWndBottomBorderSize() };

    if (SetWindowPos(_hwnd, NULL, 0, 0, wnd_size.first, wnd_size.second, (_is_visible ? SWP_SHOWWINDOW : 0) | SWP_NOMOVE | SWP_NOZORDER) == 0)
    {
        throw std::string("Can't set window size -> Error code: " + std::to_string(GetLastError()));
    }

    _size = std::move(wnd_size);
    UpdateWnd();
}

void WndBase::SetWndOrderZ(HWND hwnd)
{
    if (SetWindowPos(_hwnd, hwnd, 0, 0, 0, 0, (_is_visible ? SWP_SHOWWINDOW : 0) | SWP_NOMOVE | SWP_NOSIZE) == 0)
    {
        throw std::string("Can't set window Z-order -> Error code: " + std::to_string(GetLastError()));
    }

    UpdateWnd();
}
