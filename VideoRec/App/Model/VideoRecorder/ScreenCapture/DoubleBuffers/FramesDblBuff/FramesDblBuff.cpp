#include "FramesDblBuff.h"

void FramesDblBuff::_OnWrite(const uint8_t &index)
{
    /* Transform frame an GPU */
    _hw_accel_cl.Run(_frames[index]->data[0]/*Y*/, _frames[index]->data[1]/*U*/, _frames[index]->data[2], *_staged_buffer)/*V*/;
}

FramesDblBuff::FramesDblBuff(
    const int &src_width, const int &src_height,
    const int &dst_width, const int &dst_height)
    : _hw_accel_cl("gpu_prog.cl", "image_resample", dst_width, dst_height, src_width, src_height)
{
    /* Allocate frames */
    for (uint8_t i = 0u; i < 2u; ++i)
    {
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
        if (_frames[i])
        {
            av_frame_free(&_frames[i]);/* = NULL already */
        }
    }
}

void FramesDblBuff::SetBuffer(uint8_t **staged_buffer)
{
    _staged_buffer = staged_buffer;
}

AVFrame *FramesDblBuff::GetFrame()
{
    return _frames[_lock_index];
}
