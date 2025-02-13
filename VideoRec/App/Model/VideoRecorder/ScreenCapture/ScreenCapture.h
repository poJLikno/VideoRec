#ifndef SCREEN_CAPTURE_H_
#define SCREEN_CAPTURE_H_

#include <stdint.h>
#include <windows.h>

#include "../../../../SmtObj.h"
#include "DoubleBuffers/BitmapsDblBuff/BitmapsDblBuff.h"
#include "DoubleBuffers/FramesDblBuff/FramesDblBuff.h"
#include "DoubleBuffers/CursorsDblBuff/CursorsDblBuff.h"

class ScreenCapture
{
private:
    HWND _hwnd = nullptr;
    HDC _wnd_dev_ctx = nullptr;

    const char *_window_name;

    const bool _client_rect_only_flag;

    int _dpi = 0;

    int _src_width = 0;
    int _src_height = 0;

    int _dst_width = 0;
    int _dst_height = 0;

    SmtObj<BitmapsDblBuff> _bitmaps_dbl_buff;
    SmtObj<FramesDblBuff> _frames_dbl_buff;
    SmtObj<CursorsDblBuff> _cursors_dbl_buff;

public:
    ScreenCapture(const char *window_name, const bool &client_rect_only, const bool &use_optimization, const bool &capture_cursor, const int &dst_width = -1, const int &dst_height = -1);
    ScreenCapture(const ScreenCapture &) = delete;
    ~ScreenCapture();

    void TakeShot();
    void CaptureCursorState();

    const int &GetSrcWidth();
    const int &GetSrcHeight();
    
    const int &GetDstWidth();
    const int &GetDstHeight();

    SmtObj<BitmapsDblBuff> &GetBitmapsDblBuff();
    SmtObj<FramesDblBuff> &GetFramesDblBuff();
};

#endif /* SCREEN_CAPTURE_H_ */
