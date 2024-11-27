#ifndef FILE_MP4_H_
#define FILE_MP4_H_

#include <stdint.h>

extern "C" {
#include <libavcodec/avcodec.h>

#include <libavutil/opt.h>
#include <libavutil/imgutils.h>

#include <libavformat/avformat.h>
}

class FileMP4
{
private:
    AVFormatContext *_format_context = nullptr;
    AVStream *_stream = nullptr;
    const AVCodec *_codec;
    AVCodecContext *_codec_context = nullptr;
    AVPacket *_packet = nullptr;

    long long _frame_duration = 0ll;
    long long _frame_number = 0ll;

public:
    FileMP4(const char *file_name, const uint16_t &fps, const uint32_t &width, const uint32_t &height);
    ~FileMP4();

    void WriteFrame(AVFrame *frame);
};

#endif /* FILE_MP4_H_ */
