#include "MenuPoint.h"

#include<memory>

uint32_t MenuPoint::ID_COUNTER = WM_USER + 1;

MenuPoint::MenuPoint(const char *text, const MenuPointType &menu_point_type)
    : _text(text), _id(ID_COUNTER++), _type(menu_point_type)
{}

void MenuPoint::SetMenuParent(HMENU hmenu)
{
    std::unique_ptr<wchar_t[]> w_text(to_utf16(_text));

    AppendMenuW(hmenu, _type, _id, w_text.get());
    _parent_hmenu = hmenu;
}

unsigned int MenuPoint::GetId() const
{
    return _id;
}

bool MenuPoint::GetState() const
{
    return _state;
}

void MenuPoint::SetState(const bool &flag)
{
    _state = flag;

    switch (_type)
    {
    case Enabled:
        EnableMenuItem(_parent_hmenu, _id, (_state ? MF_ENABLED : MF_DISABLED));
        break;

    case Checked:
        CheckMenuItem(_parent_hmenu, _id, (_state ? MF_CHECKED : MF_UNCHECKED));
        break;

    default:
        break;
    }
}
