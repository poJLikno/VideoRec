#include "FileMP4.h"

#include <string>

void FileMP4::_OpenVideo()
{
    /* open the codec */
    if (avcodec_open2(_video_stream->codec_context, _video_codec, nullptr) < 0)
    {
        throw std::string("Couldn't open video codec!");
    }

    /* copy the stream parameters to the muxer */
    if (avcodec_parameters_from_context(_video_stream->stream->codecpar, _video_stream->codec_context) < 0)
    {
        throw std::string("Couldn't copy the stream parameters!");
    }
}

FileMP4::FileMP4(
    const char *file_name,
    const uint16_t &fps,
    const uint32_t &width,
    const uint32_t &height)
{
    /* Output file configuration */
    if (avformat_alloc_output_context2(&_format_context, NULL, NULL, file_name) < 0)
    {
        throw std::string("Couldn't allocate format context!");
    }
    _output_format = _format_context->oformat;

    /* Stream configuration */
    if (_output_format->video_codec != AV_CODEC_ID_NONE)
    {
        _video_stream = new OutputStream(_format_context, &_video_codec, AV_CODEC_ID_H264, fps, width, height);
    }

    /* Prepare codec */
    _OpenVideo();

    /* Open file */
    if (!(_output_format->flags & AVFMT_NOFILE))
    {
        if (avio_open(&_format_context->pb, file_name, AVIO_FLAG_WRITE) < 0)
        {
            throw std::string("Couldn't open I/O!");
        }
    }
    
    /* Write mp4 file header */
    if (avformat_write_header(_format_context, NULL) < 0)
    {
        throw std::string("Error occurred when opening output file!");
    }
}

FileMP4::~FileMP4()
{
    if (!_file_is_closed)
    {
        CloseFile();
    }

    /* Clean up others */
    _video_codec = nullptr;
    _video_stream.reset();// = nullptr;
}

void FileMP4::_WriteFrame(AVFrame *frame, SmtObj<OutputStream> &output_stream)
{
    int ret = 0;

    // send the frame to the encoder
    ret = avcodec_send_frame(output_stream->codec_context, frame);
    if (ret < 0)
    {
        throw std::string("Error sending a frame to the encoder!");
    }

    /* Recieve packet */
    while (ret >= 0) {
        ret = avcodec_receive_packet(output_stream->codec_context, output_stream->packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            break;
        }
        else if (ret < 0)
        {
            throw std::string("Error encoding a frame!");
        }

        /* rescale output packet timestamp values from codec to stream timebase */
        av_packet_rescale_ts(output_stream->packet, output_stream->codec_context->time_base, output_stream->stream->time_base);
        output_stream->packet->stream_index = output_stream->stream->index;

        /* Write the compressed frame to the media file. */
        ret = av_interleaved_write_frame(_format_context, output_stream->packet);
        /* pkt is now blank (av_interleaved_write_frame() takes ownership of
         * its contents and resets pkt), so that no unreferencing is necessary.
         * This would be different if one used av_write_frame(). */
        if (ret < 0)
        {
            throw std::string("Error while writing output packet!");
        }
    }
}

void FileMP4::WriteVideoFrame(AVFrame *frame)
{
    if (frame)
    {
        /* Set PTS for frame */
        frame->pts = _video_stream->next_pts;
        ++_video_stream->next_pts;
    }
    _WriteFrame(frame, _video_stream);
}

void FileMP4::CloseFile()
{
    /* Write null frame */
    WriteVideoFrame(nullptr);

    /* Write file ending */
    av_write_trailer(_format_context);

    /* Close streams */
    _video_stream->CloseStream();

    /* Close the output file. */
    if (!(_output_format->flags & AVFMT_NOFILE))
    {
        avio_closep(&_format_context->pb);
    }

    /* free the stream */
    avformat_close_input(&_format_context);
    avformat_free_context(_format_context);
    _format_context = nullptr;

    _file_is_closed = true;
}

FileMP4::OutputStream::OutputStream(AVFormatContext *format_context, const AVCodec **codec, AVCodecID codec_id, const int &fps, const int &width, const int &height)
{
    /* Find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec))
    {
        throw std::string("Couldn't find encoder!");
    }

    /* Allocate packet */
    packet = av_packet_alloc();
    if (!packet)
    {
        throw std::string("Couldn't allocate packet!");
    }

    /* Allocate a stream */
    stream = avformat_new_stream(format_context, NULL);
    if (!stream)
    {
        throw std::string("Couldn't allocate stream!");
    }
    stream->id = format_context->nb_streams - 1;

    /* Allocate an encoding context */
    codec_context = avcodec_alloc_context3(*codec);
    if (!codec_context)
    {
        throw std::string("Couldn't alloc an encoding context!");
    }

    /* Set video parameters */
    if ((*codec)->type == AVMEDIA_TYPE_VIDEO)
    {
        codec_context->codec_id = codec_id;

        codec_context->thread_count = 1;
        codec_context->bit_rate = 700000000ll/*700 Mb/s*/;

        /* "resolution must be a multiple of two" ??? */
        codec_context->width = width;
        codec_context->height = height;
        /* timebase: This is the fundamental unit of time (in seconds) in terms
         * of which frame timestamps are represented. For fixed-fps content,
         * timebase should be 1/framerate and timestamp increments should be
         * identical to 1. */
        codec_context->time_base = AVRational{ 1, fps };//AVRational{ 1, fps * 1000 };
        stream->time_base = codec_context->time_base;

        codec_context->gop_size = 12;/* 10 */
        codec_context->max_b_frames = 2;/* 1 */
        codec_context->pix_fmt = AV_PIX_FMT_YUV420P;

        /* Set preset for codec */
        if (codec_context->codec_id == AV_CODEC_ID_H264) {
            av_opt_set(codec_context->priv_data, "preset", "fast", 0);
            //av_opt_set(_codec_context->priv_data, "crf", "20", 0);
        }
    }

    //switch ((*codec)->type) {
    //case AVMEDIA_TYPE_AUDIO:
    //    c->sample_fmt = (*codec)->sample_fmts ?
    //        (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
    //    c->bit_rate = 64000;
    //    c->sample_rate = 44100;
    //    if ((*codec)->supported_samplerates) {
    //        c->sample_rate = (*codec)->supported_samplerates[0];
    //        for (i = 0; (*codec)->supported_samplerates[i]; i++) {
    //            if ((*codec)->supported_samplerates[i] == 44100)
    //                c->sample_rate = 44100;
    //        }
    //    }
    //    av_channel_layout_copy(&c->ch_layout, &(AVChannelLayout)AV_CHANNEL_LAYOUT_STEREO);
    //    ost->st->time_base = (AVRational){ 1, c->sample_rate };
    //    break;

    //default:
    //    break;
    //}

    /* Some formats want stream headers to be separate. */
    if (format_context->oformat->flags & AVFMT_GLOBALHEADER)
        codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

FileMP4::OutputStream::~OutputStream()
{
    if (codec_context)
    {
        CloseStream();
    }
}

void FileMP4::OutputStream::CloseStream()
{
    avcodec_free_context(&codec_context);
    codec_context = nullptr;
    av_packet_free(&packet);
    packet = nullptr;
    //swr_free(&swr_ctx);
    //swr_ctx = nullptr;
}
