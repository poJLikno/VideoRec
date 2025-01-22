#include "DShowWindow.h"

DSSafePtr<IVideoWindow> *DShowWindow::GetPtr()
{
    return &preview_window;
}

void DShowWindow::AcceptProperties()
{
    if (preview_window.object) {
        preview_window.object->put_Owner((OAHWND)wnd_metrics.owner_hwnd);
        preview_window.object->put_Width(wnd_metrics.wnd_width);
        preview_window.object->put_Height(wnd_metrics.wnd_height);
        preview_window.object->put_Left(wnd_metrics.wnd_x);
        preview_window.object->put_Top(wnd_metrics.wnd_y);
        preview_window.object->put_WindowStyle(wnd_metrics.wnd_style);
    }
}

void DShowWindow::PutOwner(HWND hWnd)
{
    if (preview_window.object) {
        preview_window.object->put_Owner((OAHWND)hWnd);
    }

    wnd_metrics.owner_hwnd = hWnd;
}

void DShowWindow::PutSize(unsigned int width, unsigned int height)
{
    if (preview_window.object) {
        preview_window.object->put_Width(width);
        preview_window.object->put_Height(height);
    }

    wnd_metrics.wnd_width = width;
    wnd_metrics.wnd_height = height;
}

void DShowWindow::PutPos(unsigned int x, unsigned int y)
{
    if (preview_window.object) {
        preview_window.object->put_Left(x);
        preview_window.object->put_Top(y);
    }

    wnd_metrics.wnd_x = x;
    wnd_metrics.wnd_y = y;
}

void DShowWindow::PutStyle(DWORD style)
{
    if (preview_window.object) {
        preview_window.object->put_WindowStyle(style);
    }

    wnd_metrics.wnd_style = style;
}

void DShowWindow::QueryMetrics(DShowWindowMetrics **metrics_ptr)
{
    *metrics_ptr = &wnd_metrics;
}
