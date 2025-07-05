#ifndef SAMPLES_BUFFER_H_
#define SAMPLES_BUFFER_H_

#include <stdint.h>

extern "C"
{
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

#include <audioclient.h>

class SamplesBuff
{
private:
    AVFrame *_frame = nullptr;

    IAudioCaptureClient *_audio_capture_client = nullptr;
    WAVEFORMATEX *_wave_format = nullptr;

    bool _no_data_flag = false;

    int _max_samples_num = 2048;

    SwrContext *_resampler_context = nullptr;

public:
    SamplesBuff(IAudioCaptureClient *audio_capture_client, WAVEFORMATEX *wave_format);
    SamplesBuff(const SamplesBuff &) = delete;
    ~SamplesBuff() ;

    void Write();

    AVFrame *GetFrame() ;
    bool GetNoDataFlag() ;
};

#endif /* SAMPLES_BUFFER_H_ */
