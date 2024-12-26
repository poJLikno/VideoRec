#include <iostream>
#include <string>

#include <windows.h>

#include "VideoRecorder/VideoRecorder.h"

/*#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")*/

int main(int argc, const char **argv)
{
    try
    {
        std::cout << "Aloha!\n";

        VideoRecorder rec;
        rec.SetNewSource(nullptr, 1920, 1080);
        rec.StartRecording("test_file.mp4", 60);
        while (GetAsyncKeyState('L') >= 0);
        rec.StopRecording();

    }
    catch (const std::string &error)
    {
        MessageBoxA(NULL, error.c_str(), "Error!", MB_OK);
    }
    
    return 0;
}
