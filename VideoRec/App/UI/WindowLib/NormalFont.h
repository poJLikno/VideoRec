#ifndef NORMAL_FONT_H_
#define NORMAL_FONT_H_

#include "WndBase.h"

class NormalFont
{
private:
    HFONT _normal_font = nullptr;

public:
    NormalFont();
    ~NormalFont();

    void SetFont(HWND hwnd);

    void SetFont(WndBase *wnd);
};

#endif /* NORMAL_FONT_H_ */
