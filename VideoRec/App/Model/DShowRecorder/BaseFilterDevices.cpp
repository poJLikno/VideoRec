#include "BaseFilterDevices.h"

#pragma warning(disable : 4996)

HRESULT Device::GetElementList(REFGUID element_category, DSSafePtr<IEnumMoniker> *element_list)
{
    HRESULT hr = S_OK;
    DSSafePtr <ICreateDevEnum> element_parser;

    // Init device parser
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&element_parser.object));
    if (FAILED(hr)) throw std::wstring(L"Can't create device parser");

    // Create an enumerator for the category.
    hr = element_parser.object->CreateClassEnumerator(element_category, &element_list->object, 0);
    if (FAILED(hr)) hr = VFW_E_NOT_FOUND;// have no devices in this category

    return hr;
}

HRESULT Device::SelectElement(DSSafePtr<IEnumMoniker> *element_list, unsigned int element_index, DSSafePtr<IBaseFilter> *element_filter)
{
    HRESULT hr = S_OK;
    DSSafePtr<IMoniker> element_handler;

    // Select device handler by index
    for (uint32_t i = 0; i < element_index + 1; i++)
    {
        hr = element_list->object->Next(1, &element_handler.object, NULL);
        if (FAILED(hr)) break;

        if (i == element_index)
        {
            // Select device
            hr = element_handler.object->BindToObject(NULL, NULL, IID_PPV_ARGS(&element_filter->object));
        }

        // Clean up
        element_handler.SafeRelease();
    }

    return hr;
}

void Device::SetCodecQuality(double video_codec_quality)
{
    HRESULT hr = E_FAIL;

    DSSafePtr<IEnumPins> enum_pins;
    DSSafePtr<IPin> pin;
    DSSafePtr<IAMVideoCompression> video_compression;

    // Find the pin that supports IAMVideoCompression (if any)
    if (device_filter.object) {
        device_filter.object->EnumPins(&enum_pins.object);
        while (S_OK == enum_pins.object->Next(1, &pin.object, NULL))
        {
            hr = pin.object->QueryInterface(IID_PPV_ARGS(&video_compression.object));
            pin.SafeRelease();
            if (SUCCEEDED(hr)) { // Found the interface.
                break;
            }
        }

        if (SUCCEEDED(hr))
        {
            long compression_params_support = 0l;
            double quality = 0.0, default_quality = 0.0;

            // Get default values and capabilities.
            hr = video_compression.object->GetInfo(
                0, 0, 0, 0,
                0, 0, &default_quality, &compression_params_support);

            if (SUCCEEDED(hr))
            {
                // Get actual values where possible.
                if (compression_params_support & CompressionCaps_CanQuality)
                {
                    video_compression.object->put_Quality(video_codec_quality);
                    hr = video_compression.object->get_Quality(&quality);
                    if (FAILED(hr) || quality < 0) {
                        quality = default_quality;
                    }
                    //std::cout << "Quality: " << quality << "\r\n";
                }
            }
        }
    }
}

void Device::GetDeviceList(wchar_t ***names_array, unsigned int *recieved_device_count)
{
    HRESULT hr = S_OK;
    DSSafePtr<IEnumMoniker> device_list;

    // Show audio devices info
    hr = GetElementList(device_class, &device_list);
    if (SUCCEEDED(hr))
    {
        DSSafePtr<IMoniker> device_handler;
        *recieved_device_count = 0;
        *names_array = (wchar_t **)calloc(1, sizeof(wchar_t *));

        while (device_list.object->Next(1, &device_handler.object, NULL) == S_OK)
        {
            DSSafePtr<IPropertyBag> device_properties;

            hr = device_handler.object->BindToStorage(NULL, NULL, IID_PPV_ARGS(&device_properties.object));
            if (FAILED(hr)) continue;

            VARIANT var = { 0 };
            VariantInit(&var);

            // Device friendly name.
            hr = device_properties.object->Read(L"FriendlyName", &var, 0);
            if (SUCCEEDED(hr))
            {
                if (*recieved_device_count > 0) {
                    *names_array = (wchar_t **)realloc(*names_array, (size_t)(*recieved_device_count + 1) * sizeof(wchar_t *));
                }
                (*names_array)[*recieved_device_count] = (wchar_t *)calloc(256, sizeof(wchar_t));
                _snwprintf((*names_array)[(*recieved_device_count)++], 256 * sizeof(wchar_t), L"%s", var.bstrVal);

                VariantClear(&var);
            }

            // Clean up
            device_handler.SafeRelease();
        }
    }
    else if (hr == VFW_E_NOT_FOUND) MessageBox(NULL, L"Can't find devices in this device category", L"Attention", MB_OK);
}

void Device::SelectDevice(int device_index)
{
    if (device_index < 0) return;

    HRESULT hr = S_OK;
    DSSafePtr<IEnumMoniker> device_list;

    // if reselect device release old object
    ReleaseDevice();

    // Select video device
    hr = GetElementList(device_class, &device_list);
    if (SUCCEEDED(hr))
    {
        hr = SelectElement(&device_list, device_index, &device_filter);
    }
    else if (hr == VFW_E_NOT_FOUND) MessageBox(NULL, L"Can't find devices in this device category", L"Attention", MB_OK);

}

void Device::ReleaseDevice()
{
    if (device_filter.object) {
        device_filter.SafeRelease();
    }
}
