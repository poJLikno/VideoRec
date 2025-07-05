#include "CheckBox.h"

CheckBox::CheckBox(const char *text, const std::pair<int, int> &pos, const std::pair<int, int> &size)
    : Button(text, pos, size)
{}

void CheckBox::SetWndParent(WndBase *wnd)
{
    std::unique_ptr<wchar_t[]> w_text(to_utf16(_text));

    _hwnd = CreateWindowExW(
        0L, L"button", w_text.get(), BS_CHECKBOX | WS_VISIBLE | WS_CHILD | WS_TABSTOP /*WS_POPUP*/,
        _pos.first, _pos.second,
        _size.first, _size.second,
        wnd->GetHwnd(), NULL, NULL, NULL);

    if (_hwnd == nullptr)
    {
        throw std::string("Can't create button -> Error code: " + std::to_string(GetLastError()));
    }

    this->_parent_wnd = wnd;
}

bool CheckBox::GetState() const
{
    return SendMessageW(_hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void CheckBox::SetState(const bool &flag)
{
    SendMessageW(_hwnd, BM_SETCHECK, (flag ? BST_CHECKED : BST_UNCHECKED), 0);
}
