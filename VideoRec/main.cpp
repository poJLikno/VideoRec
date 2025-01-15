#define APP_NAME "VideoRec"
#define APP_VERSION "2.0 Alfa"

#define WINDOWLIB_NO_MANIFEST

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


int main(int argc, const char **argv)
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    int result = 0;

    try
    {
        std::cout << "Aloha!\n";

        App app(APP_NAME, APP_VERSION);
        result = app.Run();

        std::cout << "Bye!\n";
    }
    catch (const std::string &error)
    {
        MessageBoxA(NULL, error.c_str(), "Error!", MB_OK);
    }
    
    return result;
}
