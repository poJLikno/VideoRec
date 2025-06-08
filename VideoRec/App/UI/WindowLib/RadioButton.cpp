#include "RadioButton.h"

RadioButton::RadioButton(const char *text, const bool &create_new_group, const WndPairValue &pos, const WndPairValue &size)
    : _new_group(create_new_group), Button(text, pos, size)
{
}

void RadioButton::SetWndParent(WndBase *wnd) {
    int text_size = (int)strlen(_text) + 1;
    wchar_t *w_text = new wchar_t[text_size] { 0 };
    MultiByteToWideChar(CP_UTF8, 0, _text, text_size, w_text, text_size);

    this->_parent_wnd = wnd;
    _hwnd = CreateWindowExW(0L, L"button", w_text, BS_AUTORADIOBUTTON | (_new_group ? WS_GROUP | WS_TABSTOP : 0) | WS_VISIBLE | WS_CHILD /*WS_POPUP*/,
        _pos.first, _pos.second,
        _size.first, _size.second,
        wnd->GetHwnd(), NULL, NULL, NULL);
    if (!_hwnd)
        throw std::string("Can't create button -> Error code: " + std::to_string(GetLastError()));

    delete[] w_text;
    w_text = nullptr;
}

bool RadioButton::GetState() const {
    return SendMessageW(_hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void RadioButton::SetState(const bool &flag) {
    SendMessageW(_hwnd, BM_SETCHECK, (flag ? BST_CHECKED : BST_UNCHECKED), 0);
}
