#ifndef WINDOWLIB_MENU_H_
#define WINDOWLIB_MENU_H_

#include "MenuBase.h"

class Menu : public MenuBase
{
public:
    Menu()
    {
        _hmenu = CreateMenu();
    }

    virtual void SetHParent(void *hparent) override
    {
        SetMenu((HWND)hparent, _hmenu);
    }

    // Menu point access & message detection
};

class PopupMenu : public MenuBase
{
private:
    const char *_text;

public:
    PopupMenu(const char *text) : _text(text)
    {
        _hmenu = CreatePopupMenu();
    }

    virtual void SetHParent(void *hparent) override
    {
        std::unique_ptr<wchar_t[]> w_text(to_utf16(_text));

        AppendMenuW((HMENU)hparent, MF_POPUP, (UINT_PTR)_hmenu, w_text.get());
    }
};

#endif /* WINDOWLIB_MENU_H_ */
