#include "AudioCapture.h"

#include <string>

#define REFTIMES_PER_SEC 

AudioCapture::AudioCapture()
{
    const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
    const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
    const IID IID_IAudioClient = __uuidof(IAudioClient);
    const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
    const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

    /* Init COM model for WASAPI */
    if (CoInitializeEx(NULL, COINIT_MULTITHREADED) < 0)
    {
        throw std::string("Couldn't initialize COM model!");
    }

    if (CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void **)&_device_enumerator) < 0)
    {
        throw std::string("CoCreateInstance failed!");
    }

    if (_device_enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &_device) < 0)
    {
        throw std::string("Couldn't get a loopback audio point!");
    }

    /**/
    /* Capture */
    if (_device->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void **)&_audio_client) < 0)
    {
        throw std::string("Couldn't activate device!");
    }

    /* Get a format */
    if (_audio_client->GetMixFormat(&_wave_format) < 0)
    {
        throw std::string("Couldn't get mix format!");
    }

    /* Initialize the audio client */
    if (_audio_client->Initialize(
        AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK,
        _RefTimePerSec, 0,
        _wave_format, NULL) < 0)
    {
        throw std::string("Couldn't initialize the audio client!");
    }

    // Get the size of the allocated buffer.
    if (_audio_client->GetBufferSize(&_frame_buffer_size) < 0)
    {
        throw std::string("Couldn't get the size of the allocated buffer!");

    }

    if (_audio_client->GetService(IID_IAudioCaptureClient, (void **)&_audio_capture_client) < 0)
    {
        throw std::string("Couldn't get audio capture client!");
    }

    /**/
    /* Play */
    if (_device->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void **)&_play_audio_client) < 0)
    {
        throw std::string("Couldn't activate device!");
    }

    /* Get a format */
    if (_play_audio_client->GetMixFormat(&_play_wave_format) < 0)
    {
        throw std::string("Couldn't get mix format!");
    }

    /* Initialize the audio client */
    if (_play_audio_client->Initialize(
        AUDCLNT_SHAREMODE_SHARED, 0,
        _RefTimePerSec, 0,
        _play_wave_format, NULL) < 0)
    {
        throw std::string("Couldn't initialize the audio client!");
    }

    // Get the size of the allocated buffer.
    if (_play_audio_client->GetBufferSize(&_frame_play_buffer_size) < 0)
    {
        throw std::string("Couldn't get the size of the allocated buffer!");

    }

    if (_play_audio_client->GetService(IID_IAudioRenderClient, (void **)&_audio_render_client) < 0)
    {
        throw std::string("Couldn't get audio capture client!");
    }

    

    /* Create samples buffer */
    _samples_sgl_buff = new SamplesBuff(_audio_capture_client, _wave_format);
}

AudioCapture::~AudioCapture()
{
    StopPlayAndCaptureStreams();

    _samples_sgl_buff.reset();

    /* Capture */
    CoTaskMemFree(_wave_format);

    if (_audio_capture_client)
    {
        _audio_capture_client->Release();
        _audio_capture_client = nullptr;
    }

    if (_audio_client)
    {
        _audio_client->Release();
        _audio_client = nullptr;
    }

    /* Play */
    CoTaskMemFree(_play_wave_format);

    if (_audio_render_client)
    {
        _audio_render_client->Release();
        _audio_render_client = nullptr;
    }

    if (_play_audio_client)
    {
        _play_audio_client->Release();
        _play_audio_client = nullptr;
    }

    /* Other */
    if (_device)
    {
        _device->Release();
        _device = nullptr;
    }

    if (_device_enumerator)
    {
        _device_enumerator->Release();
        _device_enumerator = nullptr;
    }

    /* Close COM model */
    CoUninitialize();
}

void AudioCapture::StartPlayAndCaptureStreams()
{
    /* Start playing */
    if (_play_audio_client->Start() < 0)
    {
        throw std::string("Couldn't start audio playing!");
    }

    /* Start recording */
    if (_audio_client->Start() < 0)
    {
        throw std::string("Couldn't start audio recording!");
    }
}

void AudioCapture::StopPlayAndCaptureStreams()
{
    _audio_client->Stop();
    _play_audio_client->Stop();
}

SmtObj<SamplesBuff> &AudioCapture::GetSamplesSglBuff()
{
    return _samples_sgl_buff;
}

void AudioCapture::CaptureAudioSamples()
{
    _samples_sgl_buff->Write();
}

WAVEFORMATEX *AudioCapture::GetWaveFormat()
{
    return _wave_format;
}
