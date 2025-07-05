#include "SamplesBuff.h"

#include <string>

void SamplesBuff::Write()
{
    uint32_t frames_num_read = 0;
    uint32_t frames_num_in_next_package = 0;
    uint32_t flags = 0;
    uint8_t *data = nullptr;

    _no_data_flag = false;

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally; make sure we do not overwrite it here */
    if (av_frame_make_writable(_frame) < 0)
    {
        throw std::string("Couldn't make frame writable!");
    }

    if (_audio_capture_client->GetNextPacketSize(&frames_num_in_next_package) < 0)
    {
        throw std::string("Couldn't get next package size!");
    }

    /* Check next frames avaibility */
    if (frames_num_in_next_package <= 0)
    {
        _no_data_flag = true;
        return;
    }

    /* Get the available data in the shared buffer */
    if (_audio_capture_client->GetBuffer(&data, &frames_num_read, (DWORD *)&flags, NULL, NULL) < 0)
    {
        throw std::string("Couldn't get shared buffer!");
    }

    _frame->nb_samples = frames_num_read;

    if (flags & AUDCLNT_BUFFERFLAGS_SILENT) /* Write silence */
    {/* Never executed in my debug tests */
        memset(_frame->data[0], 0, _frame->nb_samples * 4);
    }
    else /* Write sound */
    {
        /* convert to destination format */
        if (swr_convert(_resampler_context, (uint8_t *const *)_frame->data, _frame->nb_samples, &data, frames_num_read) < 0)
        {
            throw std::string("Error while converting samples!");
        }
    }

    if (_audio_capture_client->ReleaseBuffer(frames_num_read) < 0)
    {
        throw std::string("Couldn't release buffer!");
    }
}

SamplesBuff::SamplesBuff(IAudioCaptureClient *audio_capture_client, WAVEFORMATEX *wave_format)
    : _audio_capture_client(audio_capture_client), _wave_format(wave_format)
{
    _frame = av_frame_alloc();

    if (!_frame)
    {
        throw std::string("Couldn't allocate audio frame!");
    }

    _frame->format = AV_SAMPLE_FMT_S16; /* AV_SAMPLE_FMT_FLTP for aac codec */
    const AVChannelLayout channel_layout = AV_CHANNEL_LAYOUT_STEREO;
    av_channel_layout_copy(&_frame->ch_layout, &channel_layout);
    _frame->sample_rate = 48000;
    /* Set max samples */
    _frame->nb_samples = _max_samples_num;
    _frame->linesize[0] = _frame->nb_samples * 4;

    if (av_frame_get_buffer(_frame, 0) < 0)
    {
        throw std::string("Couldn't allocate an audio buffer!");
    }

    /* create resampler context */
    _resampler_context = swr_alloc();
    if (!_resampler_context)
    {
        throw std::string("Couldn't allocate resampler context!");
    }

    /* set options */
    av_opt_set_chlayout(_resampler_context, "in_chlayout", &channel_layout, 0);
    av_opt_set_int(_resampler_context, "in_sample_rate", 48000, 0);
    av_opt_set_sample_fmt(_resampler_context, "in_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
    av_opt_set_chlayout(_resampler_context, "out_chlayout", &channel_layout, 0);
    av_opt_set_int(_resampler_context, "out_sample_rate", 48000, 0);
    av_opt_set_sample_fmt(_resampler_context, "out_sample_fmt", (AVSampleFormat)_frame->format, 0);

    /* initialize the resampling context */
    if (swr_init(_resampler_context) < 0)
    {
        throw std::string("Couldn't initialize the resampling context!");
    }
}

SamplesBuff::~SamplesBuff()
{
    if (_frame)
    {
        _frame->nb_samples = _max_samples_num;
        av_frame_free(&_frame);/* = NULL already */
    }

    swr_free(&_resampler_context);
    _resampler_context = nullptr;

    _audio_capture_client = nullptr;
    _wave_format = nullptr;
}

AVFrame *SamplesBuff::GetFrame()
{
    return _frame;
}

bool SamplesBuff::GetNoDataFlag()
{
    return _no_data_flag;
}
