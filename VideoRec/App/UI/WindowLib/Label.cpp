#include "Label.h"

Label::Label(const char *text, const std::pair<int, int> &pos, const std::pair<int, int> &size)
    : WndBase(pos, size), _text(text)
{}

void Label::SetWndParent(WndBase *wnd)
{
    std::unique_ptr<wchar_t[]> w_text(to_utf16(_text));

    _hwnd = CreateWindowExW(
        0L, L"static", w_text.get(), WS_VISIBLE | WS_CHILD /*WS_POPUP*/,
        _pos.first, _pos.second, _size.first, _size.second,
        wnd->GetHwnd(), NULL, NULL, NULL);

    if (_hwnd == nullptr)
    {
        throw std::string("Can't create label -> Error code: " + std::to_string(GetLastError()));
    }

    _parent_wnd = wnd;
}
