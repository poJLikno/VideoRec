#ifndef BITMAPS_DOUBLE_BUFFER_H_
#define BITMAPS_DOUBLE_BUFFER_H_

#include <windows.h>

#include "../../../../../../SmtObj.h"

#include "../DoubleBuffer.h"
#include "../FramesDblBuff/FramesDblBuff.h"

class BitmapsDblBuff : public DoubleBuffer
{
protected:
    class Cursor
    {
    public:
        HCURSOR h_cursor = nullptr;
        int relative_pos_x = 0;
        int relative_pos_y = 0;
        int width = 0;
        int height = 0;
    };

    HDC _bitmaps_ctxs[2] = { 0 };
    HBITMAP _bitmaps[2] = { 0 };
    uint8_t *_dib_buffers[2] = { 0 };

    HGDIOBJ _old_objs[2] = { 0 };

    HWND _src_hwnd = nullptr;
    HDC _src_ctx = nullptr;

    const int _src_dpi;
    const int _src_width;
    const int _src_height;

    SmtObj<FramesDblBuff> &_frames_dbl_buff;

    const bool _is_window_flag;
    const bool _client_rect_only_flag;
    const bool _use_optimization_flag;
    const bool _capture_cursor_flag;

    virtual void _OnWrite(const uint8_t &index) override final;

public:
    BitmapsDblBuff(const HWND &src_hwnd, const HDC &src_ctx,
        const int &src_width, const int &src_height, const int &src_dpi,
        SmtObj<FramesDblBuff> &frames_dbl_buff,
        const bool &is_window, const bool &client_rect_only, const bool &use_optimization, const bool &capture_cursor);

    BitmapsDblBuff(const BitmapsDblBuff &) = delete;
    virtual ~BitmapsDblBuff() override;

    virtual HDC GetBitmapContext() final;
    virtual const int &GetSrcWidth() final;
    virtual const int &GetSrcHeight() final;
};

#endif
