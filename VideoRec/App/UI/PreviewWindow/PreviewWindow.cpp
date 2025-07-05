#include "PreviewWindow.h"

#include <string>

PreviewWindow::PreviewWindow(const std::pair<int, int> &pos, const std::pair<int, int> &size)
    : Window("Preview window", pos, size, -1, WS_POPUPWINDOW)
{
    /* Get DC and configure it */
    _hdc = GetDC(_hwnd);
    SetStretchBltMode(_hdc, HALFTONE);/* Make normal scale */
    _old_obj = SelectObject(_hdc, GetStockObject(WHITE_BRUSH));

    /* Create a paint timer loop */
    _paint_loop = std::unique_ptr<LoopThread>(new LoopThread([](void *ptr)->void {

        /* Draw window ~N times per second */
        PreviewWindow *this_class = (PreviewWindow *)ptr;

        /* Lock */
        this_class->_bitmaps_dbl_buff->Lock();

        /* get width and height */
        auto [wnd_client_width, wnd_client_height] = this_class->GetWndClientSize();
        auto [src_width, src_height] = this_class->_bitmaps_dbl_buff->GetSrcSize();

        /* Paint */
        StretchBlt(
            this_class->_hdc, 0, 0, wnd_client_width, wnd_client_height,
            this_class->_bitmaps_dbl_buff->GetBitmapContext(), 0, 0, src_width, src_height,
            SRCCOPY);

        /* Unlock */
        this_class->_bitmaps_dbl_buff->Unlock();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

    }, this));
}

PreviewWindow::~PreviewWindow()
{
    /* Stop preview drawing loop */
    _paint_loop.reset();

    /* Cleanup graphics */
    SelectObject(_hdc, _old_obj);
    ReleaseDC(_hwnd, _hdc);
}

void PreviewWindow::SetPreview(BitmapsDblBuff *frames_dbl_buff)
{
    if (frames_dbl_buff == nullptr)
    {
        return;
    }

    _bitmaps_dbl_buff = frames_dbl_buff;
    _paint_loop->Start();
}

void PreviewWindow::DeletePreview()
{
    _paint_loop->Stop();
    _bitmaps_dbl_buff = nullptr;

    auto [wnd_client_width, wnd_client_height] = GetWndClientSize();
    Rectangle(_hdc, 0, 0, wnd_client_width, wnd_client_height);
}
