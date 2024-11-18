#include "DoubleBuffer.h"

#include <string>

DoubleBuffer::DoubleBuffer(
    uint8_t **src_buffer,
    const int &src_width,
    const int &src_height,
    const int &dst_width,
    const int &dst_height)
    : _fmt_transform(src_buffer, src_width, src_height, dst_width, dst_height)
{
    _frames = new AVFrame *[2] { nullptr };/* Allocate frames */
    if (!_frames)
    {
        throw std::string("Couldn't allocate memmory for frames!");
    }
    
    for (uint8_t i = 0u; i < 2u; ++i)/* Allocate frames buffers */
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

DoubleBuffer::~DoubleBuffer()
{
    if (_frames)
    {
        /* Free frames*/
        for (uint8_t i = 0u; i < 2u; ++i)
        {
            av_frame_free(&_frames[i]);
            _frames[i] = nullptr;
        }

        delete[] _frames;
        _frames = nullptr;
    }
}

AVFrame *DoubleBuffer::GetFrame()
{
    return _frames[_curr_safe_frame_idx];
}

void DoubleBuffer::WriteFrame()
{
    _fmt_transform.Transform(_frames[!_curr_safe_frame_idx]);
    _curr_safe_frame_idx = (!_curr_safe_frame_idx ? 1u : 0u);
}
