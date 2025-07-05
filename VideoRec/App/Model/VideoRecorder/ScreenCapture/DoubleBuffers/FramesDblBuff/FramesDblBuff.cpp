#include "FramesDblBuff.h"

#include <string>

void FramesDblBuff::_OnWrite(const uint8_t &index)
{
    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally; make sure we do not overwrite it here */
    if (av_frame_make_writable(_frames[index]) < 0)
    {
        throw std::string("Couldn't make frame writable!");
    }
    
    /* Transform frame an GPU */
    _hw_accel_cl.Run(_frames[index]->data[0]/*Y*/, _frames[index]->data[1]/*U*/, _frames[index]->data[2]/*V*/, _staged_buffer);
}

FramesDblBuff::FramesDblBuff(const std::pair<int, int> &src_size, const std::pair<int, int> &dst_size)
    : _hw_accel_cl("gpu_prog.cl", "image_resample", dst_size, src_size)
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

        _frames[i]->width = dst_size.first;//_codec_context->width;
        _frames[i]->height = dst_size.second;//_codec_context->height;

        /* Need for manual yuv convertation (after allocation) */
        _frames[i]->linesize[0] = _frames[i]->width;

        _frames[i]->linesize[1] = _frames[i]->width / 2;
        _frames[i]->linesize[2] = _frames[i]->width / 2;

        if (av_frame_get_buffer(_frames[i], 0) < 0)
        {
            throw std::string("Couldn't allocate the video frame data!");
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

void FramesDblBuff::SetBuffer(uint8_t *staged_buffer)
{
    _staged_buffer = staged_buffer;
}

AVFrame *FramesDblBuff::GetFrame()
{
    return _frames[_lock_index];
}
