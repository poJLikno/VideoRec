#define APP_NAME "VideoRec"

#ifdef _DEBUG
#define APP_VERSION "4.0 (GDI-video) Debug"
#else
#define APP_VERSION "4.0 (GDI-video) Release"
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
*/

/* No need. DPI preference in external app manifest */
/* Very important */
/*void SetupDpiAwareness()
{
    if (!SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED))
    {
        throw std::string("SetThreadDpiAwarenessContext failed!");
    }
}
*/

int main(int argc, const char **argv)
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    int result = 0;

    try
    {
        /* No need. DPI preference in external app manifest */
        //SetupDpiAwareness();

        App app(APP_NAME, APP_VERSION);
        result = app.Run();
    }
    catch (const std::string &error)
    {
        int str_size = (int)error.length() + 1;
        SmtObj<wchar_t[]> w_error = new wchar_t[str_size] { 0 };
        MultiByteToWideChar(CP_UTF8, 0, error.c_str(), str_size, w_error, str_size);

        MessageBoxW(NULL, w_error, L"Error", MB_OK);
    }
    
    return result;
}
