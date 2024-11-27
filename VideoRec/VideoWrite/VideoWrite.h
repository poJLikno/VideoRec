#ifndef VIDEO_WRITE_H_
#define VIDEO_WRITE_H_

#include <chrono>
#include <thread>

extern "C"
{
#include <libavcodec/avcodec.h>

#include <libavutil/opt.h>
#include <libavutil/imgutils.h>

#include <libavformat/avformat.h>
}

#include "../DoubleBuffer/DoubleBuffer.h"
#include "FileMP4.h"

class VideoWrite
{
private:
    std::thread *_video_write_thrd;
    DoubleBuffer _double_buffer;
    int _width = 0;
    int _height = 0;

    static void _VideoWriteLoop(const char *file_name, const uint16_t &fps, const int &width, const int &height);

public:
    VideoWrite(uint8_t *src_buffer, const int &src_width, const int &src_height, const int &dst_width, const int &dst_height);

    void StartWrite(const char *file_name, const uint16_t &fps);
    //void StopWrite();
};

#endif /* VIDEO_WRITE_H_ */
