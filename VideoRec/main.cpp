#define APP_NAME "VideoRec"
#define APP_VERSION "2.5 (video-only) Release"

#define WINDOWLIB_USE_MANIFEST
//#define WINDOWLIB_NO_CONSOLE

#include <iostream>
#include <string>

#include "App/App.h"

/*
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")

#pragma comment(lib, "OpenCL.lib")
*/

/* Very important */
void SetupDpiAwareness()
{
    //if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
    if (!SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED))
        throw std::string("SetThreadDpiAwarenessContext failed!");
}

int main(int argc, const char **argv)
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    int result = 0;

    try
    {
        SetupDpiAwareness();

        App app(APP_NAME, APP_VERSION);
        result = app.Run();
    }
    catch (const std::string &error)
    {
        MessageBoxA(NULL, error.c_str(), "Error!", MB_OK);
    }
    
    return result;
}
