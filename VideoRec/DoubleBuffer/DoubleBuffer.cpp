#include "DoubleBuffer.h"

#include <string>

DoubleBuffer::DoubleBuffer(
    uint8_t *src_buffer,
    const int &width,
    const int &height)
    : _fmt_transform(src_buffer, width, height)
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
        _frames[i]->width = width;//_codec_context->width;
        _frames[i]->height = height;//_codec_context->height;

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

void DoubleBuffer::LockFrame()
{
    if (_frame_is_locked)
    {
        throw std::string("Frame has already been locked!");
    }

    _frame_is_locked = true;
    _has_new_frame = false;
}

void DoubleBuffer::UnlockFrame()
{
    if (!_frame_is_locked)
    {
        throw std::string("Frame has already been unlocked!");
    }


    if (_has_new_frame)
    {
        _curr_safe_frame_idx = (!_curr_safe_frame_idx ? 1u : 0u);
    }

    _frame_is_locked = false;
}

AVFrame *DoubleBuffer::GetFrame()
{
    return _frames[_curr_safe_frame_idx];
}

void DoubleBuffer::WriteFrame()
{
    bool frame_is_locked = _frame_is_locked;

    _fmt_transform.Transform(_frames[!_curr_safe_frame_idx]);

    if (frame_is_locked)
    {
        _has_new_frame = true;
    }
    else
    {
        _curr_safe_frame_idx = (!_curr_safe_frame_idx ? 1u : 0u);
    }
}
