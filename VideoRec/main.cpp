#include <iostream>
#include <string>

#include <windows.h>

#include "ScreenCapture/ScreenCapture.h"
#include "DoubleBuffer/DoubleBuffer.h"
#include "IntegralTimer/IntegralTimer.h"

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")

ScreenCapture screen_capture(nullptr, 800, 600);
DoubleBuffer frames_buffer(screen_capture.GetScreenShotBuffer(), 800, 600);

void foo(void *)
{
    screen_capture.TakeShot();
    frames_buffer.WriteFrame();
}

int main(int argc, const char **argv)
{
    try
    {
        std::cout << "Aloha!\n";

        

        IntegralTimer timer_a(30, foo);
        timer_a.Start();
        while (GetAsyncKeyState('L') >= 0);
        timer_a.Stop();
    }
    catch (std::string &error)
    {
        MessageBoxA(NULL, error.c_str(), "Error!", MB_OK);
    }
    
    return 0;
}
