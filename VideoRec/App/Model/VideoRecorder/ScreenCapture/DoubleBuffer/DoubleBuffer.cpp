#include "DoubleBuffer.h"

#include <string>

DoubleBuffer::DoubleBuffer(
    const int &src_width,
    const int &src_height,
    const int &dst_width,
    const int &dst_height)
    : _hw_accel_cl("gpu_prog.cl", "image_resample", dst_width, dst_height, src_width, src_height)
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

DoubleBuffer::~DoubleBuffer()
{
    if (_frames)
    {

        /* Free frames*/
        for (uint8_t i = 0u; i < 2u; ++i)
        {
            av_frame_free(&_frames[i]);/* = NULL already */
        }

        delete[] _frames;
        _frames = nullptr;
    }
}

void DoubleBuffer::LockFrame()
{
    uint8_t local_newest_index = _newest_index;
    if (_mutexs[local_newest_index].try_lock())
    {
        _lock_index = local_newest_index;
    }
    else
    {
        _mutexs[(local_newest_index ^ (1<<0))].lock();
        _lock_index = local_newest_index ^ (1<<0);
    }
}

void DoubleBuffer::UnlockFrame()
{
    _mutexs[_lock_index].unlock();
}

AVFrame *DoubleBuffer::GetFrame()
{
    return _frames[_lock_index];
}

void DoubleBuffer::WriteFrame(uint8_t *src_buffer)
{
    if (_mutexs[(_newest_index ^ (1<<0))].try_lock())
    {
        _hw_accel_cl.Run(_frames[(_newest_index ^ (1<<0))]->data[0]/*Y*/, _frames[(_newest_index ^ (1<<0))]->data[1]/*U*/, _frames[(_newest_index ^ (1<<0))]->data[2], src_buffer)/*V*/;

        _newest_index ^= (1<<0);
        _mutexs[_newest_index].unlock();
    }
    else
    {
        _mutexs[_newest_index].lock();

        _hw_accel_cl.Run(_frames[_newest_index]->data[0]/*Y*/, _frames[_newest_index]->data[1]/*U*/, _frames[_newest_index]->data[2], src_buffer)/*V*/;

        _mutexs[_newest_index].unlock();
    }
}
