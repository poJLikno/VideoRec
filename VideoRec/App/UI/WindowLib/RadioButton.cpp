#include "RadioButton.h"

RadioButton::RadioButton(const char *text, const bool &create_new_group, const std::pair<int, int> &pos, const std::pair<int, int> &size)
    : _new_group(create_new_group), Button(text, pos, size)
{}

void RadioButton::SetWndParent(WndBase *wnd)
{
    std::unique_ptr<wchar_t[]> w_text(to_utf16(_text));

    _hwnd = CreateWindowExW(
        0L, L"button", w_text.get(), BS_AUTORADIOBUTTON | (_new_group ? WS_GROUP | WS_TABSTOP : 0) | WS_VISIBLE | WS_CHILD /*WS_POPUP*/,
        _pos.first, _pos.second,
        _size.first, _size.second,
        wnd->GetHwnd(), NULL, NULL, NULL);

    if (_hwnd == nullptr)
    {
        throw std::string("Can't create button -> Error code: " + std::to_string(GetLastError()));
    }
    
    _parent_wnd = wnd;
}

bool RadioButton::GetState() const
{
    return SendMessageW(_hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void RadioButton::SetState(const bool &flag)
{
    SendMessageW(_hwnd, BM_SETCHECK, (flag ? BST_CHECKED : BST_UNCHECKED), 0);
}
