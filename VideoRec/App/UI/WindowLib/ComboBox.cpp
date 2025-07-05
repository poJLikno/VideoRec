#include "ComboBox.h"

ComboBox::ComboBox(const std::pair<int, int> &pos, const std::pair<int, int> &size)
    : WndBase(pos, size)
{}

void ComboBox::SetWndParent(WndBase *wnd)
{
    _hwnd = CreateWindowExW(
        0L, L"combobox", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP,
        _pos.first, _pos.second,
        _size.first, _size.second,
        wnd->GetHwnd(), NULL, NULL, NULL);

    if (_hwnd == nullptr)
    {
        throw std::string("Can't create combobox -> Error code: " + std::to_string(GetLastError()));
    }

    _parent_wnd = wnd;
}

void ComboBox::AddItem(const char *text)
{
    std::unique_ptr<wchar_t[]> w_text(to_utf16(text));

    SendMsg(CB_ADDSTRING, NULL, (LPARAM)w_text.get());
}

void ComboBox::SelectItem(const int &id)
{
    SendMsg(CB_SETCURSEL, (WPARAM)id, NULL);
}

int ComboBox::GetItemId()
{
    return (int)SendMsg(CB_GETCURSEL, NULL, NULL);
}

/* Number of symbols (wide char) without null ending */
int ComboBox::GetItemLength()
{
    return (int)SendMsg(CB_GETLBTEXTLEN, (WPARAM)GetItemId(), NULL);
}

/* Need free after use (delete[] ...) */
char *ComboBox::GetItem(int &bytes_were_written)
{
    std::unique_ptr<wchar_t[]> w_buffer = std::make_unique<wchar_t[]>(GetItemLength() + 1);

    SendMsg(CB_GETLBTEXT, (WPARAM)GetItemId(), (LPARAM)w_buffer.get());

    return to_utf8(w_buffer.get(), bytes_were_written);
}

/* Need free after use (delete[] ...) */
char *ComboBox::GetItem()
{
    int tmp = 0;
    return GetItem(tmp);
}

/* Need free after use (delete[] ...) */
char *ComboBox::GetOldItem(const int &max_symbols_count/* include null-symbol */)
{
    std::unique_ptr<wchar_t[]> w_buffer = std::make_unique<wchar_t[]>(max_symbols_count);

    GetWindowTextW(_hwnd, w_buffer.get(), max_symbols_count);

    return to_utf8(w_buffer.get());
}
