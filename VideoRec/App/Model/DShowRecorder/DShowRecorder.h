#ifndef DSHOW_RECORDER_H
#define DSHOW_RECORDER_H

#include <windows.h>

#include "BaseFilterDevices.h"
#include "../../UI/DShowWindow/DShowWindow.h"

class DShowRecorder
{
private:
    DSSafePtr<IGraphBuilder> graph;
    DSSafePtr<ICaptureGraphBuilder2> graph_builder;

    VideoDevice video_device;
    VideoCodec video_codec;
    AudioDevice audio_device;
    AudioCodec audio_codec;

    DShowWindow *preview_window = nullptr;

    bool is_running = false;

    bool video_filter_settings = false;
    bool video_pin_settings = false;
    bool audio_filter_settings = false;
    bool audio_pin_settings = false;
    bool recording_allow = false;
    bool &preview_allow;
    

    void InitGraph(DSSafePtr<IGraphBuilder> *graph, DSSafePtr<ICaptureGraphBuilder2> *graph_builder);
    void RegisterDevice(DSSafePtr<IBaseFilter> *device, const wchar_t *device_name);
    void SetPreview();
    void SetOutputFile(DSSafePtr<IBaseFilter> *avi_mux_filter);
    void SetStreams(DSSafePtr<IBaseFilter> *avi_mux_filter);
    void StartStream(DSSafePtr<IGraphBuilder> *graph);
    void StopStream(DSSafePtr<IGraphBuilder> *graph);
    void ShowFilterSettings(DSSafePtr<IBaseFilter> *device);
    void ShowPinSettings(DSSafePtr<IBaseFilter> *device);

    void Start();
    void Stop();

    //void AttachDevice(Device *device_ptr);

    void ShowVideoFilterSettings(bool state);
    void ShowVideoPinSettings(bool state);
    void ShowAudioFilterSettings(bool state);
    void ShowAudioPinSettings(bool state);

    void AllowPreview(bool state);
    void AllowRecording(bool state);

public:
    DShowRecorder(bool &preview_allow_flag);
    ~DShowRecorder();

    bool HasFilterSettings(Device *device);
    bool HasPinSettings(Device *device);

    double GetDefaultVideoCodecQuality();

    /**/

    void AttachDShowWindow(DShowWindow *wnd_ptr);
    void StartRecording();
    void StopRecording();
    void SelectDevice(unsigned int index, Device *device);
    //void ShowPreview();
    //void HidePreview();
    void ShowVideoFilterSettings();
    void ShowVideoPinSettings();
    void ShowAudioFilterSettings();
    void ShowAudioPinSettings();

    VideoDevice *GetVideoDevice();
    VideoCodec *GetVideoCodec();
    AudioDevice *GetAudioDevice();
    AudioCodec *GetAudioCodec();
};

#endif /* DSHOW_RECORDER_H */
