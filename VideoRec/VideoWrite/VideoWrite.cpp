#include "VideoWrite.h"

#include <string>

void VideoWrite::_VideoWriteLoop(
    const char *file_name,
    const uint16_t &fps,
    const int &width,
    const int &height)
{
    FileMP4 file(file_name, fps, width, height);
}

VideoWrite::VideoWrite(
    uint8_t *src_buffer,
    const int &src_width,
    const int &src_height,
    const int &dst_width,
    const int &dst_height)
    : _double_buffer(src_buffer, dst_width, dst_height)
{

}

void VideoWrite::StartWrite(const char *file_name, const uint16_t &fps)
{
    if (_video_write_thrd)
    {
        delete _video_write_thrd;
        _video_write_thrd = nullptr;
    }

    _video_write_thrd = new std::thread(_VideoWriteLoop, file_name, fps, _width, _height);
    _video_write_thrd->detach();

}
