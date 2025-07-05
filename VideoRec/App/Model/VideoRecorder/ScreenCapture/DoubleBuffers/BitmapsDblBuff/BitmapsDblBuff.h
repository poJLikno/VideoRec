#ifndef BITMAPS_DOUBLE_BUFFER_H_
#define BITMAPS_DOUBLE_BUFFER_H_

#include <memory>
#include <windows.h>

#include "../DoubleBuffer.h"
#include "../FramesDblBuff/FramesDblBuff.h"
#include "../CursorsDblBuff/CursorsDblBuff.h"

class BitmapsDblBuff : public DoubleBuffer
{
protected:
    HDC _bitmaps_ctxs[2] = { 0 };
    HBITMAP _bitmaps[2] = { 0 };
    uint8_t *_dib_buffers[2] = { 0 };

    HGDIOBJ _old_objs[2] = { 0 };

    HWND _src_hwnd;
    HDC _src_ctx;
    const std::pair<int, int> _src_size;

    std::unique_ptr<FramesDblBuff> &_frames_dbl_buff;
    std::unique_ptr<CursorsDblBuff> &_cursors_dbl_buff;

    const bool _use_optimization_flag;
    const bool _capture_cursor_flag;

    virtual void _OnWrite(const uint8_t &index) override final;

public:
    BitmapsDblBuff(
        HWND src_hwnd, HDC src_ctx,
        const std::pair<int, int> &src_size,
        std::unique_ptr<FramesDblBuff> &frames_dbl_buff,
        std::unique_ptr<CursorsDblBuff> &cursors_dbl_buff,
        const bool &use_optimization, const bool &capture_cursor);
    BitmapsDblBuff(const BitmapsDblBuff &) = delete;
    virtual ~BitmapsDblBuff() override;

    virtual std::pair<int, int> GetSrcSize() final;

    /* Need lock */
    virtual HDC GetBitmapContext() final;
};

#endif
