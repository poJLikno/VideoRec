#include "DShowRecorder.h"

#include <thread>

#pragma warning(disable : 4996)

void DShowRecorder::InitGraph(DSSafePtr<IGraphBuilder> *graph, DSSafePtr<ICaptureGraphBuilder2> *graph_builder)
{
    HRESULT hr = S_OK;

    // Create the Filter Graph Manager.
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&graph->object);
    if (FAILED(hr)) throw std::wstring(L"Can't create Graph");

    // Create the Capture Graph Builder.
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void **)&graph_builder->object);
    if (FAILED(hr)) throw std::wstring(L"Can't create GraphBuilder");

    graph_builder->object->SetFiltergraph(graph->object);
}

void DShowRecorder::RegisterDevice(DSSafePtr<IBaseFilter> *device, const wchar_t *device_name)
{
    if (video_device.GetPtr()->object)
        graph.object->AddFilter(device->object, device_name);
}

void DShowRecorder::SetPreview()
{
    if (!video_device.GetPtr()->object) return;
    
    HRESULT hr = graph.object->QueryInterface(IID_PPV_ARGS(&preview_window->GetPtr()->object));
    if (FAILED(hr)) throw std::wstring(L"Can't get preview window interface");

    hr = graph_builder.object->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, video_device.GetPtr()->object, NULL, NULL);
    if (FAILED(hr)) throw std::wstring(L"Can't render video preview stream");

    preview_window->AcceptProperties();
}

void DShowRecorder::SetOutputFile(DSSafePtr<IBaseFilter> *avi_mux_filter)
{
    if (!video_device.GetPtr()->object && !audio_device.GetPtr()->object) return;
    
    HRESULT hr = S_OK;

    size_t thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
    SYSTEMTIME record_start_time = { 0 };
    GetLocalTime(&record_start_time);
    std::string a(std::getenv("USERNAME"));
    std::wstring user_name(a.begin(), a.end());

    std::wstring file_name(
        L"C:\\Users\\" + user_name
        + L"\\Videos\\Recording" + std::to_wstring(thread_id) + L"_"
        + std::to_wstring(record_start_time.wDay) + L"-"
        + std::to_wstring(record_start_time.wMonth) + L"-"
        + std::to_wstring(record_start_time.wYear) + L"_"
        + std::to_wstring(record_start_time.wHour) + L"-"
        + std::to_wstring(record_start_time.wMinute) + L"-"
        + std::to_wstring(record_start_time.wSecond)
        + L".avi");

    hr = graph_builder.object->SetOutputFileName(&MEDIASUBTYPE_Avi, file_name.c_str(), &avi_mux_filter->object, NULL);
    if (FAILED(hr)) throw std::wstring(L"Can't set output file name");
}

void DShowRecorder::SetStreams(DSSafePtr<IBaseFilter> *avi_mux_filter)
{
    // Set video stream
    if (video_device.GetPtr()->object)
    {
        RegisterDevice(video_codec.GetPtr(), L"Video Codec Filter");
        HRESULT hr = graph_builder.object->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, video_device.GetPtr()->object, video_codec.GetPtr()->object, avi_mux_filter->object);
        if (FAILED(hr)) throw std::wstring(L"Can't render video stream");
    }

    // Set audio stream
    if (audio_device.GetPtr()->object)
    {
        RegisterDevice(audio_codec.GetPtr(), L"Audio Codec Filter");
        HRESULT hr = graph_builder.object->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, audio_device.GetPtr()->object, audio_codec.GetPtr()->object, avi_mux_filter->object);
        if (FAILED(hr)) throw std::wstring(L"Can't render audio stream");
    }

    if (!video_device.GetPtr()->object || !audio_device.GetPtr()->object) return;

    HRESULT hr = S_OK;
    DSSafePtr<IConfigAviMux> config_avi_mux;
    DSSafePtr<IConfigInterleaving> config_interleaveing;

    // Set main stream (audio stream prefered)
    hr = avi_mux_filter->object->QueryInterface(IID_PPV_ARGS(&config_avi_mux.object));
    if (SUCCEEDED(hr))
    {
        config_avi_mux.object->SetMasterStream(1);
    }

    // Set interleaving mode
    hr = avi_mux_filter->object->QueryInterface(IID_PPV_ARGS(&config_interleaveing.object));
    if (SUCCEEDED(hr))
    {
        config_interleaveing.object->put_Mode(INTERLEAVE_NONE);//INTERLEAVE_NONE);
    }
}

void DShowRecorder::StartStream(DSSafePtr<IGraphBuilder> *graph)
{
    DSSafePtr<IMediaControl> media_control;

    graph->object->QueryInterface(IID_PPV_ARGS(&media_control.object));
    media_control.object->Run();
}

void DShowRecorder::StopStream(DSSafePtr<IGraphBuilder> *graph)
{
    DSSafePtr<IMediaControl> media_control;

    graph->object->QueryInterface(IID_PPV_ARGS(&media_control.object));
    media_control.object->Stop();
}

void DShowRecorder::ShowFilterSettings(DSSafePtr<IBaseFilter> *device)
{
    if (device->object)
    {
        HRESULT hr = S_OK;
        DSSafePtr<ISpecifyPropertyPages> specify_property_pages;
        CAUUID cauuid = { 0 };

        hr = device->object->QueryInterface(IID_PPV_ARGS(&specify_property_pages.object));
        if (SUCCEEDED(hr))
        {
            hr = specify_property_pages.object->GetPages(&cauuid);
            if (SUCCEEDED(hr) && cauuid.cElems > 0)
            {
                OleCreatePropertyFrame(NULL, 30, 30, NULL, 1, (IUnknown **)&device->object, cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL);

                CoTaskMemFree(cauuid.pElems);
            }
        }
    }
}

void DShowRecorder::ShowPinSettings(DSSafePtr<IBaseFilter> *device)
{
    if (device->object && graph_builder.object)
    {
        HRESULT hr = S_OK;
        DSSafePtr<IAMStreamConfig> video_stream_config;
        DSSafePtr<ISpecifyPropertyPages> specify_property_pages;
        CAUUID cauuid = { 0 };

        hr = graph_builder.object->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, device->object, IID_PPV_ARGS(&video_stream_config.object));
        if (FAILED(hr))
        {
            hr = graph_builder.object->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, device->object, IID_PPV_ARGS(&video_stream_config.object));
        }

        if (SUCCEEDED(hr))
        {
            hr = video_stream_config.object->QueryInterface(IID_PPV_ARGS(&specify_property_pages.object));
            if (SUCCEEDED(hr))
            {
                hr = specify_property_pages.object->GetPages(&cauuid);
                if (SUCCEEDED(hr) && cauuid.cElems > 0)
                {
                    OleCreatePropertyFrame(NULL, 30, 30, NULL, 1, (IUnknown **)&video_stream_config.object, cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL);

                    CoTaskMemFree(cauuid.pElems);
                }
            }
        }
    }
}

double DShowRecorder::GetDefaultVideoCodecQuality()
{
    double default_quality = -1.0;

    long compression_params_support = 0l;

    HRESULT hr = E_FAIL;

    DSSafePtr<IEnumPins> enum_pins;
    DSSafePtr<IPin> pin;
    DSSafePtr<IAMVideoCompression> video_compression;

    // Find the pin that supports IAMVideoCompression (if any)
    if (video_codec.GetPtr()->object) {
        video_codec.GetPtr()->object->EnumPins(&enum_pins.object);
        while (S_OK == enum_pins.object->Next(1, &pin.object, NULL))
        {
            hr = pin.object->QueryInterface(IID_PPV_ARGS(&video_compression.object));
            pin.SafeRelease();
            if (SUCCEEDED(hr)) // Found the interface.
            {
                break;
            }
        }

        if (SUCCEEDED(hr)) {
            // Get default values and capabilities.
            hr = video_compression.object->GetInfo(
                0, 0, 0, 0,
                0, 0, &default_quality, &compression_params_support);

            /*if (compression_params_support & CompressionCaps_CanQuality) {
                std::cout << "Default Quality: " << default_quality << "\r\n";
            }
            else std::cout << "Unsupported\r\n";*/
        }
    }

    return default_quality;
}

void DShowRecorder::StartRecording()
{
    Stop();
    AllowRecording(true);
    Start();
}

void DShowRecorder::StopRecording()
{
    Stop();
    AllowRecording(false);
    Start();
}

void DShowRecorder::SelectDevice(unsigned int index, Device *device)
{
    if (index < 0) return;

    Stop();
    device->SelectDevice(index);
    Start();
}

//void DShowRecorder::ShowPreview()
//{
//    Stop();
//    AllowPreview(true);
//    Start();
//}
//
//void DShowRecorder::HidePreview()
//{
//    Stop();
//    AllowPreview(false);
//    Start();
//}

void DShowRecorder::ShowVideoFilterSettings()
{
    Stop();
    ShowVideoFilterSettings(true);
    Start();
    ShowVideoFilterSettings(false);
}

void DShowRecorder::ShowVideoPinSettings()
{
    Stop();
    ShowVideoPinSettings(true);
    Start();
    ShowVideoPinSettings(false);
}

void DShowRecorder::ShowAudioFilterSettings()
{
    Stop();
    ShowAudioFilterSettings(true);
    Start();
    ShowAudioFilterSettings(false);
}

void DShowRecorder::ShowAudioPinSettings()
{
    Stop();
    ShowAudioPinSettings(true);
    Start();
    ShowAudioPinSettings(false);
}

VideoDevice *DShowRecorder::GetVideoDevice()
{
    return &video_device;
}

VideoCodec *DShowRecorder::GetVideoCodec()
{
    return &video_codec;
}

AudioDevice *DShowRecorder::GetAudioDevice()
{
    return &audio_device;
}

AudioCodec *DShowRecorder::GetAudioCodec()
{
    return &audio_codec;
}

void DShowRecorder::Start()
{
    if (is_running) return;
    is_running = true;

    HRESULT hr = S_OK;

    preview_window->GetPtr()->SafeRelease();

    graph.SafeRelease();
    graph_builder.SafeRelease();

    InitGraph(&graph, &graph_builder);

    // Video device
    RegisterDevice(video_device.GetPtr(), L"Video Capture Filter");
    if (video_filter_settings) {
        ShowFilterSettings(video_device.GetPtr());
    }
    if (video_pin_settings) {
        ShowPinSettings(video_device.GetPtr());
    }

    // Audio device
    RegisterDevice(audio_device.GetPtr(), L"Audio Capture Filter");
    if (audio_filter_settings) {
        ShowFilterSettings(audio_device.GetPtr());
    }
    if (audio_pin_settings) {
        ShowPinSettings(audio_device.GetPtr());
    }

    // Set video preview
    if (preview_allow) {
        SetPreview();
    }

    // Set recording
    if (recording_allow)
    {
        DSSafePtr<IBaseFilter> avi_mux_filter;

        // Set output file
        SetOutputFile(&avi_mux_filter);

        // Streams settings
        SetStreams(&avi_mux_filter);
    }

    StartStream(&graph);
}

void DShowRecorder::Stop()
{
    if (!is_running) return;
    is_running = false;

    StopStream(&graph);
}

//void DShowRecorder::AttachDevice(Device *device_ptr)
//{
//    if (dynamic_cast<VideoDevice *>(device_ptr)) {
//        video_device = dynamic_cast<VideoDevice *>(device_ptr);
//    }
//    else if (dynamic_cast<VideoCodec *>(device_ptr)) {
//        video_codec = dynamic_cast<VideoCodec *>(device_ptr);
//    }
//    else if (dynamic_cast<AudioDevice *>(device_ptr)) {
//        audio_device = dynamic_cast<AudioDevice *>(device_ptr);
//    }
//    else if (dynamic_cast<AudioCodec *>(device_ptr)) {
//        audio_codec = dynamic_cast<AudioCodec *>(device_ptr);
//    }
//}

void DShowRecorder::AttachDShowWindow(DShowWindow *wnd_ptr)
{
    preview_window = wnd_ptr;
}

void DShowRecorder::ShowVideoFilterSettings(bool state)
{
    video_filter_settings = state;
}

void DShowRecorder::ShowVideoPinSettings(bool state)
{
    video_pin_settings = state;
}

void DShowRecorder::ShowAudioFilterSettings(bool state)
{
    audio_filter_settings = state;
}

void DShowRecorder::ShowAudioPinSettings(bool state)
{
    audio_pin_settings = state;
}

void DShowRecorder::AllowPreview(bool state)
{
    preview_allow = state;
}

void DShowRecorder::AllowRecording(bool state)
{
    recording_allow = state;
}

DShowRecorder::DShowRecorder(bool &preview_allow_flag)
    : preview_allow(preview_allow_flag)
{
    // COM init
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY);
    if (FAILED(hr)) throw std::runtime_error("Can't initialize COM");
}

DShowRecorder::~DShowRecorder()
{
    /* COM release */
    CoUninitialize();
}

bool DShowRecorder::HasFilterSettings(Device *device)
{
    bool result = false;

    if (device->GetPtr()->object)
    {
        HRESULT hr = S_OK;
        DSSafePtr<ISpecifyPropertyPages> specify_property_pages;
        CAUUID cauuid = { 0 };

        hr = device->GetPtr()->object->QueryInterface(IID_PPV_ARGS(&specify_property_pages.object));
        if (SUCCEEDED(hr))
        {
            hr = specify_property_pages.object->GetPages(&cauuid);
            if (SUCCEEDED(hr) && cauuid.cElems > 0) result = true;
        }
    }

    return result;
}

bool DShowRecorder::HasPinSettings(Device *device)
{
    bool result = false;

    if (device->GetPtr()->object && graph_builder.object)
    {
        HRESULT hr = S_OK;
        DSSafePtr<IAMStreamConfig> video_stream_config;
        DSSafePtr<ISpecifyPropertyPages> specify_property_pages;
        CAUUID cauuid = { 0 };

        hr = graph_builder.object->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, device->GetPtr()->object, IID_PPV_ARGS(&video_stream_config.object));
        if (FAILED(hr))
        {
            hr = graph_builder.object->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, device->GetPtr()->object, IID_PPV_ARGS(&video_stream_config.object));
        }

        if (SUCCEEDED(hr))
        {
            hr = video_stream_config.object->QueryInterface(IID_PPV_ARGS(&specify_property_pages.object));
            if (SUCCEEDED(hr))
            {
                hr = specify_property_pages.object->GetPages(&cauuid);
                if (SUCCEEDED(hr) && cauuid.cElems > 0) result = true;
            }
        }
    }

    return result;
}
