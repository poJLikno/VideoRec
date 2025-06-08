#ifndef AUDIO_CAPTURE_H_
#define AUDIO_CAPTURE_H_

#include <stdint.h>

#include <mmdeviceapi.h>
#include <uuids.h>
#include <audioclient.h>

/* #pragma comment(lib, "ole32.lib") */

#include "../../../../SmtObj.h"
#include "SamplesBuff/SamplesBuff.h"

class AudioCapture
{
private:
    const uint32_t _RefTimePerSec = 10000000; /* 1 sec */

    IMMDeviceEnumerator *_device_enumerator = nullptr;
    IMMDevice *_device = nullptr;

    /* Capture */
    IAudioClient *_audio_client = nullptr;
    IAudioCaptureClient *_audio_capture_client = nullptr;
    WAVEFORMATEX *_wave_format = nullptr;
    uint32_t _frame_buffer_size = 0u;
    /**/

    SmtObj<SamplesBuff> _samples_sgl_buff;

    /* Play */
    IAudioClient *_play_audio_client = nullptr;
    IAudioRenderClient *_audio_render_client = nullptr;
    WAVEFORMATEX *_play_wave_format = nullptr;
    uint32_t _frame_play_buffer_size = 0u;
    /**/

public:
    AudioCapture();
    AudioCapture(const AudioCapture &) = delete;
    ~AudioCapture();

    void StartPlayAndCaptureStreams();
    void StopPlayAndCaptureStreams();

    SmtObj<SamplesBuff> &GetSamplesSglBuff();

    void CaptureAudioSamples();

    WAVEFORMATEX *GetWaveFormat();
};

#endif /* AUDIO_CAPTURE_H_ */
