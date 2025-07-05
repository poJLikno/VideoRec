#ifndef SCREEN_CAPTURE_H_
#define SCREEN_CAPTURE_H_

#include <memory>
#include <stdint.h>
#include <windows.h>

#include "DoubleBuffers/BitmapsDblBuff/BitmapsDblBuff.h"
#include "DoubleBuffers/FramesDblBuff/FramesDblBuff.h"
#include "DoubleBuffers/CursorsDblBuff/CursorsDblBuff.h"

class ScreenCapture
{
private:
    HWND _hwnd = nullptr;
    HDC _wnd_dev_ctx = nullptr;

    const char *_window_name;

    std::pair<int, int> _src_size = { 0, 0 };
    std::pair<int, int> _dst_size = { 0, 0 };

    std::unique_ptr<BitmapsDblBuff> _bitmaps_dbl_buff;
    std::unique_ptr<FramesDblBuff> _frames_dbl_buff;
    std::unique_ptr<CursorsDblBuff> _cursors_dbl_buff;

public:
    ScreenCapture(const char *window_name, const bool &client_rect_only, const bool &use_optimization, const bool &capture_cursor, const std::pair<int, int> &dst_size = { -1, -1});
    ScreenCapture(const ScreenCapture &) = delete;
    ~ScreenCapture();

    void CaptureScreenFrame();
    void CaptureCursorState();

    std::pair<int, int> GetSrcSize();
    
    std::pair<int, int> GetDstSize();

    std::unique_ptr<BitmapsDblBuff> &GetBitmapsDblBuff();
    std::unique_ptr<FramesDblBuff> &GetFramesDblBuff();
};

#endif /* SCREEN_CAPTURE_H_ */
