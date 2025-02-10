#ifndef SCREEN_CAPTURE_H_
#define SCREEN_CAPTURE_H_

#include <stdint.h>
#include <windows.h>

#include "DoubleBuffer/DoubleBuffer.h"

class ScreenCapture
{
private:
    HWND _hwnd = nullptr;
    HDC _wnd_dev_ctx = nullptr;
    BITMAPINFO _bitmap_info = { 0 };
    HDC _bitmap_ctx = nullptr;
    HBITMAP _bitmap = nullptr;
    HGDIOBJ _old_obj = nullptr;

    const bool &_client_rect_only_flag;
    bool &_use_optimization_flag;
    bool &_capture_cursor_flag;

    uint8_t *_buffer = nullptr;

    int _dpi = 0;

    int _src_width = 0;
    int _src_height = 0;

    int _dst_width = 0;
    int _dst_height = 0;

    SmtObj<DoubleBuffer> _frames_buffer;

    void _DrawCursor();

public:
    ScreenCapture(const char *window_name, const bool &client_rect_only, bool &use_optimization, bool &capture_cursor, const int &dst_width = -1, const int &dst_height = -1);
    ScreenCapture(const ScreenCapture &) = delete;
    ~ScreenCapture();

    void TakeShot();

    const int &GetSrcWidth();
    const int &GetSrcHeight();

    const int &GetDstWidth();
    const int &GetDstHeight();

    DoubleBuffer *GetFramesBuffer();

    HDC GetBitmapContextForPreview();
};

#endif /* SCREEN_CAPTURE_H_ */
