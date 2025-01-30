#define APP_NAME "VideoRec"
#define APP_VERSION "2.5 (video-only) Release"

#define WINDOWLIB_USE_MANIFEST
//#define WINDOWLIB_NO_CONSOLE

#include <iostream>
#include <string>
#include <mutex>
//#include <threads.h>

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

//class A
//{
//private:
//    std::mutex _mutexs[2] = {};
//    int lock_index = 0;
//    int newest_index = 1;
//    SmtObj<int[]> _buff;
//
//public:
//    A()
//    {
//        _buff = new int[2]{ 0 };
//    }
//    A(const A &) = delete;
//    A(A &&) = delete;
//
//    void Set(const int &data)
//    {
//        if (_mutexs[!newest_index].try_lock())
//        {
//            _buff[!newest_index] = data;
//            std::cout << "SSSSet to " << !newest_index << "\n";
//            newest_index = (newest_index ? 0 : 1);
//
//            _mutexs[newest_index].unlock();
//        }
//        else
//        {
//            _mutexs[newest_index].lock();
//            _buff[newest_index] = data;
//            std::cout << "SSSSSSSSet to " << newest_index << "\n";
//            newest_index = (newest_index ? 0 : 1);
//            _mutexs[!newest_index].unlock();
//        }
//
//    }
//
//    void Lock()
//    {
//        int local_newest_index = newest_index;
//        if (_mutexs[local_newest_index].try_lock())
//        {
//            lock_index = local_newest_index;
//            //_mutex_0.unlock();
//        }
//        else
//        {
//            _mutexs[!local_newest_index].lock();
//            lock_index = (local_newest_index ? 0 : 1);
//            std::cout << "-- ";
//            //_mutex_1.unlock();
//        }
//    }
//
//    void Unlock()
//    {
//        _mutexs[lock_index].unlock();
//    }
//
//    const int &Get()
//    {
//        return _buff[lock_index];
//    }
//
//    const int &GetFrom()
//    {
//        return lock_index;
//    }
//};
//
//int main(int argc, const char **argv)
//{
//    SetConsoleCP(65001);
//    SetConsoleOutputCP(65001);
//
//    int result = 0;
//
//    try
//    {
//        SetupDpiAwareness();
//
//        A buff;
//        bool flag = true;
//
//        std::thread setter1_thr([&](){
//            while (flag)
//            {
//                buff.Set(1);
//                std::this_thread::sleep_for(std::chrono::milliseconds(1));
//            }
//            });
//
//        std::thread getter_thr([&](){
//            while (flag)
//            {
//                buff.Lock();
//                std::cout << "Get from " << buff.GetFrom() << "\n";
//                buff.Unlock();
//                std::this_thread::sleep_for(std::chrono::milliseconds(1));
//            }
//            });
//
//        while (GetAsyncKeyState('L') >= 0) std::this_thread::sleep_for(std::chrono::milliseconds(50));
//
//        flag = false;
//
//        setter1_thr.join();
//        getter_thr.join();
//
//        std::system("pause");
//
//    }
//    catch (const std::string &error)
//    {
//        MessageBoxA(NULL, error.c_str(), "Error!", MB_OK);
//    }
//
//    return result;
//}
