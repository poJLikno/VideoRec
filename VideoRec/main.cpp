#define APP_NAME "VideoRec"

#ifdef _DEBUG
#define APP_VERSION "4.2 (GDI-video) Debug"
#else
#define APP_VERSION "4.2 (GDI-video) Release"
#endif

/* Also uses external app manifest */
#define WINDOWLIB_USE_MANIFEST
#define WINDOWLIB_NO_CONSOLE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>

#include "App/App.h"

/*
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")

#pragma comment(lib, "OpenCL.lib")

#pragma comment(lib, "ole32.lib")
*/

/* Using -O2 (speed) optimization */

int main(int argc, const char **argv)
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    int result = 0;

    try
    {
        App app(APP_NAME, APP_VERSION);
        result = app.Run();
    }
    catch (const std::string &error)
    {
        std::unique_ptr<wchar_t[]> w_error(to_utf16(error.c_str()));
        MessageBoxW(NULL, w_error.get(), L"Error", MB_OK);
    }
    
    return result;
}
