#ifndef BASE_FILTER_DEVICES_H
#define BASE_FILTER_DEVICES_H

#include <iostream>
#include <string>
#include <windows.h>

#include "DSSafePtr.h"

class Device
{
protected:
    DSSafePtr<IBaseFilter> device_filter;
    REFGUID device_class;

    HRESULT GetElementList(REFGUID element_category, DSSafePtr<IEnumMoniker> *element_list);
    HRESULT SelectElement(DSSafePtr<IEnumMoniker> *element_list, unsigned int element_index, DSSafePtr<IBaseFilter> *element_filter);

    void SetCodecQuality(double video_codec_quality);

    Device(REFGUID device_class_) : device_class(device_class_) {}

public:
    virtual ~Device() {}

    // Free every list element before free list pointer. Use free();
    void GetDeviceList(wchar_t ***device_names_array, unsigned int *recieved_device_count);// Pay attention! may memory leaks!
    void SelectDevice(int device_index);
    void ReleaseDevice();

    DSSafePtr<IBaseFilter> *GetPtr() {
        return &device_filter;
    }
};

class VideoDevice : public Device
{
public:
    VideoDevice() : Device(CLSID_VideoInputDeviceCategory) {}
};

class VideoCodec : public Device
{
public:
    using Device::SetCodecQuality;
    VideoCodec() : Device(CLSID_VideoCompressorCategory) {}
};

class AudioDevice : public Device
{
public:
    AudioDevice() : Device(CLSID_AudioInputDeviceCategory) {}
};

class AudioCodec : public Device
{
public:
    AudioCodec() : Device(CLSID_AudioCompressorCategory) {}
};

#endif
