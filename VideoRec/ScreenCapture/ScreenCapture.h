#ifndef SCREEN_CAPTURE_H_
#define SCREEN_CAPTURE_H_

#include <stdint.h>
#include <windows.h>

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


public:
    ScreenCapture(const char *window_name, const int &dst_width, const int &dst_height);
    ~ScreenCapture();

    void TakeShot();

    int GetDstWidth();
    int GetDstHeight();

    uint8_t *GetScreenShotBuffer();
};

#endif /* SCREEN_CAPTURE_H_ */
