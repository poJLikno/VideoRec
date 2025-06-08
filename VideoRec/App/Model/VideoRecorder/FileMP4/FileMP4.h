#ifndef FILE_MP4_H_
#define FILE_MP4_H_

#include <stdint.h>
#include <stdio.h>

#include <windows.h>
#include <audioclient.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
}

#include "../../../../SmtObj.h"

class FileMP4
{
private:
    /* Wrapper around a single output AVStream */
    class OutputStream
    {
    public:
        AVStream *stream = nullptr;
        AVCodecContext *codec_context = nullptr;

        /* pts of the next frame that will be generated */
        uint64_t next_pts = 0ull;

        AVPacket *packet;

        OutputStream(AVFormatContext *format_context, const AVCodec **codec, AVCodecID codec_id,
            const int &fps = 0,
            const int &width = 0, const int &height = 0);

        ~OutputStream();

        void CloseStream();
    };

    AVFormatContext *_format_context = nullptr;

    SmtObj<OutputStream> _video_stream;
    const AVCodec *_video_codec;

    SmtObj<OutputStream> _audio_stream;
    const AVCodec *_audio_codec;

    bool _file_is_closed = false;

    void _OpenCodec(SmtObj<OutputStream> &output_stream, const AVCodec *codec);
    void _WriteFrame(AVFrame *frame, SmtObj<OutputStream> &output_stream);

public:
    FileMP4(const char *file_name, const uint16_t &fps, const uint32_t &width, const uint32_t &height, WAVEFORMATEX *wave_format);
    FileMP4(const FileMP4 &) = delete;
    ~FileMP4();

    SmtObj<OutputStream> &GetVideoStream();
    SmtObj<OutputStream> &GetAudioStream();

    void WriteVideoFrame(AVFrame *frame);
    void WriteAudioFrame(AVFrame *frame);
    void CloseFile();
};

#endif /* FILE_MP4_H_ */
