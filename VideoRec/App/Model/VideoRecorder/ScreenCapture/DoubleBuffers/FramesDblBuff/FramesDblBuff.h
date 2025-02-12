#ifndef FRAMES_DOUBLE_BUFFER_H_
#define FRAMES_DOUBLE_BUFFER_H_

#include <windows.h>

#include "../CursorsDblBuff/CursorsDblBuff.h"

#include "HWAccelCL.h"

extern "C"
{
#include <libswresample/swresample.h>
}

class FramesDblBuff : public DoubleBuffer
{
protected:
    HDC _bitmaps_ctxs[2] = { 0 };
    HBITMAP _bitmaps[2] = { 0 };
    uint8_t *_dib_buffers[2] = { 0 };

    HGDIOBJ _old_objs[2] = { 0 };

    HWND _src_hwnd = nullptr;
    HDC _src_ctx = nullptr;
    int _src_width = 0;
    int _src_height = 0;

    SmtObj<CursorsDblBuff> &_cursors_dbl_buff;

    const bool _use_optimization_flag;
    const bool _capture_cursor_flag;

    AVFrame *_frames[2] = { 0 };
    HWAccelCL _hw_accel_cl;

    virtual void _OnWrite(const uint8_t &index) override final;

public:
    FramesDblBuff(const HWND &src_hwnd, const HDC &src_ctx,
        const int &src_width, const int &src_height,
        const int &dst_width, const int &dst_height,
        SmtObj<CursorsDblBuff> &cursors_dbl_buff,
        const bool &use_optimization, const bool &capture_cursor);
    FramesDblBuff(const FramesDblBuff &) = delete;
    virtual ~FramesDblBuff() override;

    virtual AVFrame *GetFrame() final;
    virtual HDC GetFrameContext() final;
    virtual int GetSrcWidth() final;
    virtual int GetSrcHeight() final;
};

#endif /* FRAMES_DOUBLE_BUFFER_H_ */
