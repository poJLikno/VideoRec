#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include "../WindowLib/Window.h"

class MainWindow : public Window
{
private:
    static Window *_MainWnd;

    static LRESULT __stdcall _MainWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
    MainWindow(
        const char *name,
        const std::pair<int, int> &pos = { CW_USEDEFAULT, CW_USEDEFAULT },
        const std::pair<int, int> &size = { CW_USEDEFAULT, CW_USEDEFAULT },
        const int &icon_id = -1,
        const unsigned long &wnd_style = WS_OVERLAPPEDWINDOW);

    virtual ~MainWindow() override;
};

#endif /* MAIN_WINDOW_H_ */
