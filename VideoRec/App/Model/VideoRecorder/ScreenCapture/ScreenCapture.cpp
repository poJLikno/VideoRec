#include "ScreenCapture.h"

#include <string>

ScreenCapture::ScreenCapture(const int &dst_width, const int &dst_height)
    :_dst_width(dst_width), _dst_height(dst_height)
{
    // Get desktop
    HDESK desktop = OpenInputDesktop(0, FALSE, GENERIC_ALL);
    if (!desktop)
    {
        throw std::string("Failed to open desktop!");
    }

    // Attach desktop to this thread (presumably for cases where this is not the main/UI thread)
    bool is_desktop_attached = SetThreadDesktop(desktop) != 0;
    CloseDesktop(desktop);
    desktop = nullptr;
    if (!is_desktop_attached)
    {
        throw std::string("Failed to attach recording thread to desktop!");
    }

    // Initialize DirectX
    // Driver types supported
    D3D_DRIVER_TYPE driver_types[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    size_t num_driver_types = ARRAYSIZE(driver_types);

    // Feature levels supported
    D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_1
    };
    uint32_t num_feature_levels = ARRAYSIZE(feature_levels);

    D3D_FEATURE_LEVEL feature_level;

    // Create device
    for (size_t i = 0; i < num_driver_types; ++i)
    {
        _hr = D3D11CreateDevice(nullptr, driver_types[i], nullptr, 0, feature_levels, (UINT)num_feature_levels,
            D3D11_SDK_VERSION, &_d3d11_device, &feature_level, &_d3d11_device_context);
        if (SUCCEEDED(_hr))
        {
            break;
        }
    }
    if (FAILED(_hr))
    {
        throw std::string("Failed to create d3d11 device!");
    }

    // Initialize the Desktop Duplication system
    // Get DXGI device
    IDXGIDevice *dxgi_device = nullptr;
    _hr = _d3d11_device->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxgi_device);
    if (FAILED(_hr))
    {
        throw std::string("Failed to get dxgi device!");
    }

    // Get DXGI adapter
    IDXGIAdapter *dxgi_adapter = nullptr;
    _hr = dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void **)&dxgi_adapter);
    dxgi_device->Release();
    dxgi_device = nullptr;
    if (FAILED(_hr))
    {
        throw std::string("Failed to get dxgi adapter!");
    }

    // Get output
    IDXGIOutput *dxgi_output = nullptr;
    _hr = dxgi_adapter->EnumOutputs(0u/*OutputNumber*/, &dxgi_output);
    dxgi_adapter->Release();
    dxgi_adapter = nullptr;
    if (FAILED(_hr))
    {
        throw std::string("Failed to get output!");
    }

    dxgi_output->GetDesc(&_output_desc);

    // QI for Output 1
    IDXGIOutput1 *dxgi_output_1 = nullptr;
    _hr = dxgi_output->QueryInterface(__uuidof(dxgi_output_1), (void **)&dxgi_output_1);
    dxgi_output->Release();
    dxgi_output = nullptr;
    if (FAILED(_hr))
    {
        throw std::string("Failed to get output 1!");
    }

    // Create desktop duplication
    _hr = dxgi_output_1->DuplicateOutput(_d3d11_device, &_desktop_dupl);
    dxgi_output_1->Release();
    dxgi_output_1 = nullptr;
    if (FAILED(_hr))
    {
        if (_hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
        {
            throw std::string("Too many desktop recorders already active!");
        }
        throw std::string("Failed to create desktop duplication!");
    }

    /* Get first frame for src width and height */
    TakeShot();

    /* Create double buffer */
    //_frames_buffer = new DoubleBuffer(_src_width, _src_height, _dst_width, _dst_height);
}

ScreenCapture::~ScreenCapture()
{
    /* Release resources */
    if (_desktop_dupl)
    {
        _desktop_dupl->Release();
        _desktop_dupl = nullptr;
    }

    if (_d3d11_device_context)
    {
        _d3d11_device_context->Release();
        _d3d11_device_context = nullptr;
    }

    if (_d3d11_device)
    {
        _d3d11_device->Release();
        _d3d11_device = nullptr;
    }
}

void ScreenCapture::TakeShot()
{
    if (!_desktop_dupl)
    {
        throw std::string("Failture to check a source");
    }

    /*
    according to the docs, it's best for performance if we hang onto the frame for as long as possible,
    and only release the previous frame immediately before acquiring the next one. Something about
    the OS coalescing updates, so that it doesn't have to store them as distinct things.
    */
    if (_has_frame_lock)
    {
        _has_frame_lock = false;
        _desktop_dupl->ReleaseFrame();
    }

    /* Get the next frame */
    IDXGIResource *desktop_resource = nullptr;
    DXGI_OUTDUPL_FRAME_INFO frame_info;
    _hr = _desktop_dupl->AcquireNextFrame(0, &frame_info, &desktop_resource);
    if (_hr == DXGI_ERROR_WAIT_TIMEOUT)
    {
        return;
    }
    else if (FAILED(_hr))
    {
        throw std::string("Failture to acquire the next frame!");
    }

    /* Lock the frame */
    _has_frame_lock = true;

    ID3D11Texture2D *gpu_texture = nullptr;
    _hr = desktop_resource->QueryInterface(__uuidof(ID3D11Texture2D), (void **)&gpu_texture);
    desktop_resource->Release();
    desktop_resource = nullptr;
    if (FAILED(_hr))
    {
        /* ? */
        return;
    }

    D3D11_TEXTURE2D_DESC texture_desc;
    gpu_texture->GetDesc(&texture_desc);
    texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
    texture_desc.Usage = D3D11_USAGE_STAGING;
    texture_desc.BindFlags = 0;
    texture_desc.MiscFlags = 0; // D3D11_RESOURCE_MISC_GDI_COMPATIBLE ?
    ID3D11Texture2D *cpu_texture = nullptr;
    _hr = _d3d11_device->CreateTexture2D(&texture_desc, nullptr, &cpu_texture);
    if (FAILED(_hr))
    {
        /* ? */
        return;
    }

    _d3d11_device_context->CopyResource(cpu_texture, gpu_texture);

    D3D11_MAPPED_SUBRESOURCE mapped_subresource;
    _hr = _d3d11_device_context->Map(cpu_texture, 0, D3D11_MAP_READ, 0, &mapped_subresource);
    if (SUCCEEDED(_hr))
    {
        if (_src_width != texture_desc.Width || _src_height != texture_desc.Height) {
            _src_width = texture_desc.Width;
            _src_height = texture_desc.Height;
            if (_dst_width == -1)
            {
                _dst_width = _src_width;
            }
            if (_dst_height == -1)
            {
                _dst_height = _src_height;
            }
            _frames_buffer = new DoubleBuffer(_src_width, _src_height, _dst_width, _dst_height);
        }

        _frames_buffer->WriteFrame((uint8_t *)mapped_subresource.pData);

        _d3d11_device_context->Unmap(cpu_texture, 0);
    }

    cpu_texture->Release();
    gpu_texture->Release();
}

const int &ScreenCapture::GetSrcWidth()
{
    return _src_width;
}

const int &ScreenCapture::GetSrcHeight()
{
    return _src_height;
}

const int &ScreenCapture::GetDstWidth()
{
    return _dst_width;
}

const int &ScreenCapture::GetDstHeight()
{
    return _dst_height;
}

DoubleBuffer *ScreenCapture::GetFramesBuffer()
{
    return _frames_buffer;
}
