#ifndef DSHOW_WINDOW_H
#define DSHOW_WINDOW_H

#include <iostream>
#include <windows.h>

#include "../../Model/DShowRecorder/DSSafePtr.h"

class DShowWindowMetrics
{
public:
    HWND owner_hwnd = { 0 };
    unsigned int wnd_width = 0u;
    unsigned int wnd_height = 0u;
    unsigned int wnd_x = 0u;
    unsigned int wnd_y = 0u;
    DWORD wnd_style = 0u;
};

class DShowWindow
{
private:
    DSSafePtr<IVideoWindow> preview_window;
    DShowWindowMetrics wnd_metrics;

public:
    DSSafePtr<IVideoWindow> *GetPtr();

    void AcceptProperties();

    void PutOwner(HWND hWnd);
    void PutSize(unsigned int width, unsigned int height);
    void PutPos(unsigned int x, unsigned int y);
    void PutStyle(DWORD style);

    void QueryMetrics(DShowWindowMetrics **metrics_ptr);
};

#endif /* DSHOW_WINDOW_H */
