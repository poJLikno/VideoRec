#include "CheckBox.h"

CheckBox::CheckBox(const char *text, const WndPairValue &pos, const WndPairValue &size)
    : Button(text, pos, size)
{
}

void CheckBox::SetWndParent(WndBase *wnd) {
    int text_size = (int)strlen(_text) + 1;
    wchar_t *w_text = new wchar_t[text_size] { 0 };
    MultiByteToWideChar(CP_UTF8, 0, _text, text_size, w_text, text_size);

    this->_parent_wnd = wnd;
    _hwnd = CreateWindowExW(0L, L"button", w_text, BS_CHECKBOX | WS_VISIBLE | WS_CHILD | WS_TABSTOP /*WS_POPUP*/,
        _pos.first, _pos.second,
        _size.first, _size.second,
        wnd->GetHwnd(), NULL, NULL, NULL);
    if (!_hwnd)
        throw std::string("Can't create button -> Error code: " + std::to_string(GetLastError()));

    delete[] w_text;
    w_text = nullptr;
}

bool CheckBox::GetState() const {
    return SendMessageW(_hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void CheckBox::SetState(const bool &flag) {
    SendMessageW(_hwnd, BM_SETCHECK, (flag ? BST_CHECKED : BST_UNCHECKED), 0);
}
