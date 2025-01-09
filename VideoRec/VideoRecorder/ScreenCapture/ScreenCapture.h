#ifndef SCREEN_CAPTURE_H_
#define SCREEN_CAPTURE_H_

#include <stdint.h>
#include <windows.h>

#include "DoubleBuffer/DoubleBuffer.h"

class ScreenCapture
{
private:
    HWND _hwnd = { 0 };
    HDC _wnd_dev_ctx = { 0 };
    BITMAPINFOHEADER _bitmap_info = { 0 };
    HDC _bitmap_ctx = { 0 };
    HBITMAP _bitmap = { 0 };
    HGDIOBJ _old_obj = { 0 };

    uint8_t *_buffer = nullptr;

    int _src_width = 0;
    int _src_height = 0;
    int _dst_width = 0;
    int _dst_height = 0;

    SmtObj<DoubleBuffer> _frames_buffer;

public:
    ScreenCapture(const char *window_name, const int &dst_width = -1, const int &dst_height = -1);
    ScreenCapture(const ScreenCapture &) = delete;
    ~ScreenCapture();

    void TakeShot();

    const int &GetDstWidth();
    const int &GetDstHeight();

    DoubleBuffer &FramesBuffer();
};

#endif /* SCREEN_CAPTURE_H_ */
