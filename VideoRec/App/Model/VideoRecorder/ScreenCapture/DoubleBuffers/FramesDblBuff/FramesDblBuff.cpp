#include "FramesDblBuff.h"

void FramesDblBuff::_OnWrite(const uint8_t &index)
{
    /* Get captured frame */
    if (_use_optimization_flag)
    {
        BitBlt(_bitmaps_ctxs[index], 0, 0, _src_width, _src_height, _src_ctx, 0, 0, SRCCOPY);
    }
    else
    {
        PrintWindow(_src_hwnd, _bitmaps_ctxs[index], PW_RENDERFULLCONTENT);
    }

    /* Draw a cursor */
    if (_capture_cursor_flag)
    {
        _cursors_dbl_buff->Lock();
        Cursor *cursor = _cursors_dbl_buff->Get();

        DrawIconEx(
            _bitmaps_ctxs[index],/* dst */
            cursor->relative_pos_x,/* x */
            cursor->relative_pos_y,/* y */
            cursor->h_cursor,/* src */
            cursor->width,/* width */
            cursor->height,/* height */
            0, NULL, DI_NORMAL);

        _cursors_dbl_buff->Unlock();
    }

    /* Transform frame an GPU */
    _hw_accel_cl.Run(_frames[index]->data[0]/*Y*/, _frames[index]->data[1]/*U*/, _frames[index]->data[2], _dib_buffers[index])/*V*/;
}

FramesDblBuff::FramesDblBuff(const HWND &src_hwnd, const HDC &src_ctx,
    const int &src_width, const int &src_height,
    const int &dst_width, const int &dst_height,
    SmtObj<CursorsDblBuff> &cursors_dbl_buff,
    const bool &use_optimization, const bool &capture_cursor)
    : _src_hwnd(src_hwnd), _src_ctx(src_ctx),
    _src_width(src_width), _src_height(src_height),
    _cursors_dbl_buff(cursors_dbl_buff),
    _use_optimization_flag(use_optimization), _capture_cursor_flag(capture_cursor),
    _hw_accel_cl("gpu_prog.cl", "image_resample", dst_width, dst_height, src_width, src_height)
{
    /* Fill bitmap information */
    BITMAPINFO _bitmap_info = { 0 };
    _bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    _bitmap_info.bmiHeader.biWidth = _src_width;
    _bitmap_info.bmiHeader.biHeight = -(_src_height);/* Make top-down DIB (upper-left corner) */
    _bitmap_info.bmiHeader.biBitCount = 32u;
    _bitmap_info.bmiHeader.biSizeImage = _src_width * _src_height * 4;
    _bitmap_info.bmiHeader.biPlanes = 1u;
    _bitmap_info.bmiHeader.biCompression = BI_RGB;
    _bitmap_info.bmiHeader.biClrUsed = 0u;
    _bitmap_info.bmiHeader.biClrImportant = 0u;
    _bitmap_info.bmiHeader.biXPelsPerMeter = 0;
    _bitmap_info.bmiHeader.biYPelsPerMeter = 0;

    /* Allocate bitmaps and frames */
    for (uint8_t i = 0u; i < 2u; ++i)
    {
        /* Create bitmap */
        _bitmaps_ctxs[i] = CreateCompatibleDC(nullptr);/* Set arg to null like in OBS code */

        //_bitmap = CreateCompatibleBitmap(_wnd_dev_ctx, _src_width, _src_height);
        //GetDIBits()

        _bitmaps[i] = CreateDIBSection(_src_ctx, (const BITMAPINFO *)&_bitmap_info, DIB_RGB_COLORS, (void **)&_dib_buffers[i], nullptr, 0);
        if (_bitmaps[i] == nullptr) {
            throw std::string("Couldn't allocate frame buffer!");
        }

        _old_objs[i] = SelectObject(_bitmaps_ctxs[i], (HGDIOBJ)_bitmaps[i]);


        _frames[i] = av_frame_alloc();
        if (!_frames[i])
        {
            throw std::string("Couldn't allocate video frame!");
        }

        /* Set frame parameters */
        _frames[i]->format = AV_PIX_FMT_YUV420P;//_codec_context->pix_fmt;

        _frames[i]->width = dst_width;//_codec_context->width;
        _frames[i]->height = dst_height;//_codec_context->height;

        /* Need for manual yuv convertation (after allocation) */
        _frames[i]->linesize[0] = _frames[i]->width;

        _frames[i]->linesize[1] = _frames[i]->width / 2;
        _frames[i]->linesize[2] = _frames[i]->width / 2;

        if (av_frame_get_buffer(_frames[i], 0) < 0)
        {
            throw std::string("Couldn't allocate the video frame data!");
        }

        if (av_frame_make_writable(_frames[i]) < 0)
        {
            throw std::string("Couldn't make frame writable!");
        }
    }
}

FramesDblBuff::~FramesDblBuff()
{
    /* Free frames */
    for (uint8_t i = 0u; i < 2u; ++i)
    {
        SelectObject(_bitmaps_ctxs[i], _old_objs[i]);
        DeleteDC(_bitmaps_ctxs[i]);
        DeleteObject((HGDIOBJ)_bitmaps[i]);/* Also release memory for buffer */

        //_dib_buffers[i] = nullptr;/* GDI will release bitmap buffer */

        if (_frames[i])
        {
            av_frame_free(&_frames[i]);/* = NULL already */
        }
    }
}

AVFrame *FramesDblBuff::GetFrame()
{
    return _frames[_lock_index];
}

HDC FramesDblBuff::GetFrameContext()
{
    return _bitmaps_ctxs[_lock_index];
}

int FramesDblBuff::GetSrcWidth()
{
    return _src_width;
}

int FramesDblBuff::GetSrcHeight()
{
    return _src_height;
}
