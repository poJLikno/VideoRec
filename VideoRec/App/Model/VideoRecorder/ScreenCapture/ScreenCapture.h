#ifndef SCREEN_CAPTURE_H_
#define SCREEN_CAPTURE_H_

#include <SDKDDKVer.h>

#include <stdint.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>
#include <dxgi1_2.h>

#include "DoubleBuffer/DoubleBuffer.h"

class ScreenCapture
{
private:
    HRESULT _hr = S_OK;

    ID3D11Device *_d3d11_device = nullptr;
    ID3D11DeviceContext *_d3d11_device_context = nullptr;
    IDXGIOutputDuplication *_desktop_dupl = nullptr;
    DXGI_OUTPUT_DESC _output_desc;
    bool _has_frame_lock = false;

    int _src_width = 0;
    int _src_height = 0;
    int _dst_width = 0;
    int _dst_height = 0;

    SmtObj<DoubleBuffer> _frames_buffer;

public:
    ScreenCapture(const int &dst_width = -1, const int &dst_height = -1);
    ScreenCapture(const ScreenCapture &) = delete;
    ~ScreenCapture();

    void TakeShot();

    const int &GetSrcWidth();
    const int &GetSrcHeight();

    const int &GetDstWidth();
    const int &GetDstHeight();

    DoubleBuffer *GetFramesBuffer();
};

#endif /* SCREEN_CAPTURE_H_ */
