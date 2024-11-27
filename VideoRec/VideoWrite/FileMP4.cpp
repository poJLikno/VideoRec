#include "FileMP4.h"

#include <string>

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

    /* Stream configuration */
    _stream = avformat_new_stream(_format_context, NULL);
    if (_stream = 0)
    {
        throw std::string("Couldn't create new stream!");
    }

    /* Codec configuration */
    _codec = avcodec_find_encoder_by_name("mpeg4");
    if (_codec == 0)
    {
        //if ((_codec = avcodec_find_encoder(AV_CODEC_ID_H264)) == 0) {
        throw std::string("Couldn't set codec!");
    }

    /* Allocate memory for codec context */
    if ((_codec_context = avcodec_alloc_context3(_codec)) == 0)
    {
        throw std::string("Couldn't allocate video codec context!");
    }

    /* Set preset for codec */
    if (_codec->id == AV_CODEC_ID_H264) {
        //if (av_opt_set(codec_context->priv_data, "preset", "slow", 0)) {// zero on success
        //	fprintf(stderr, "could not set options\n");
        //	exit(1);
        //}
        av_opt_set(_codec_context->priv_data, "preset", "fast", 0);
        av_opt_set(_codec_context->priv_data, "crf", "20", 0);
    }

    /* Configure codec framerate and fps */

    {


        //av_opt_set(_codec_context->priv_data, "preset", "slow", 0);

        _codec_context->thread_count = 1;
        _codec_context->bit_rate = 400000;
        /* resolution must be a multiple of two */
        _codec_context->width = width;
        _codec_context->height = height;
        _codec_context->time_base = AVRational{ 1, fps * 1000 };
        _codec_context->framerate = AVRational{ fps, 1 };
        /* emit one intra frame every ten frames
         * check frame pict_type before passing frame
         * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
         * then gop_size is ignored and the output of encoder
         * will always be I frame irrespective to gop_size
         */
        _codec_context->gop_size = 10;
        _codec_context->max_b_frames = 1;
        _codec_context->pix_fmt = AV_PIX_FMT_YUV420P;
    }
    _stream->time_base = _codec_context->time_base;

    /* ??? */
    if (_format_context->oformat->flags & AVFMT_GLOBALHEADER)
    {
        _codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    /* Prepare codec */
    if (avcodec_open2(_codec_context, _codec, NULL) < 0)
    {
        throw std::string("Couldn't open codec!");
    }

    /* Apply codec parametors to stream */
    if (avcodec_parameters_from_context(_stream->codecpar, _codec_context) < 0)
    {
        throw std::string("Couldn't set stream codec parameters!");
    }

    /* Open file */
    if (avio_open(&_format_context->pb, file_name, AVIO_FLAG_WRITE) < 0)
    {
        throw std::string("Couldn't open I/O!");
    }

    /* Write mp4 file header */
    avformat_write_header(_format_context, NULL);/* No check out */

    /* Set frame duration */
    _frame_duration = static_cast<long long>(_codec_context->time_base.den * _codec_context->framerate.den);
    _frame_duration /= static_cast<long long>(_codec_context->time_base.num * _codec_context->framerate.num);

    /* Allocate packet for frames */
    _packet = av_packet_alloc();
    if (_packet == 0)
    {
        throw std::string("Couldn't allocate packet!");
    }
}

FileMP4::~FileMP4()
{
    /* Flush the encoder & Flush the rest of the packets */
    _frame_duration = 0ll;
    WriteFrame(nullptr);

    /* Write file ending */
    av_write_trailer(_format_context);

    /* Free frame buffer & packet */
    /*av_frame_free(&_frame);
    _frame = nullptr;*/
    av_packet_free(&_packet);
    _packet = nullptr;

    /* Close file */
    avformat_close_input(&_format_context);

    /* Free file context & codec context */
    avformat_free_context(_format_context);
    _format_context = nullptr;
    avcodec_free_context(&_codec_context);
    _codec_context = nullptr;

    /* Clean up others */
    _codec = nullptr;
    _stream = nullptr;
    _frame_number = 0ll;
}

void FileMP4::WriteFrame(AVFrame *frame)
{
    /* Set frame time stamp */
    if (frame)
    {
        frame->pts = _frame_number++ * _frame_duration;
    }

    /* Send frame */
    if (avcodec_send_frame(_codec_context, frame) < 0)
    {
        throw std::string("Couldn't send frame!");
    }

    /* Recieve packet*/
    int result = 0;
    do
    {
        result = avcodec_receive_packet(_codec_context, _packet);
        _packet->duration = _frame_duration;

        /* Write frame */
        av_write_frame(_format_context, _packet);//av_interleaved_write_frame(avfc, pkt);
        av_packet_unref(_packet);
    } while (result >= 0);

    //++_frame_number;
    if (result == AVERROR(EAGAIN) || result == AVERROR_EOF)
    {
        return;
    }

    else if (result < 0)
    {
        throw std::string("Error during encoding!");
    }

    /* Increase frame number */
}
