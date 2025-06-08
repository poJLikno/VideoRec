#include "LoopThread.h"

#include <string>

#include <windows.h>

void LoopThread::_ActionLoop(LoopThread *loop_thread)
{
    try
    {
        bool tmp = false;

        while (true)
        {
            tmp = loop_thread->_run_flag.load(std::memory_order_relaxed);
            std::atomic_thread_fence(std::memory_order_acquire);
            if (!tmp)
            {
                break;
            }

            if (loop_thread->_loop_action)
            {
                loop_thread->_loop_action(loop_thread->_action_parameters);
            }
        }
    }
    catch (const std::string &error)/* Exeption termintion */
    {
        int str_size = (int)error.length() + 1;
        SmtObj<wchar_t[]> w_error = new wchar_t[str_size] { 0 };
        MultiByteToWideChar(CP_UTF8, 0, error.c_str(), str_size, w_error, str_size);

        MessageBoxW(NULL, w_error, L"Error", MB_OK);
    }
}

LoopThread::LoopThread(void(*function)(void *), void *func_parameters)
    : _loop_action(function), _action_parameters(func_parameters)
{}

LoopThread::~LoopThread()
{
    Stop();
}

void LoopThread::Start()
{
    if (_loop_thread)
    {
        throw std::string("LoopThread has been already started!");
    }

    /* Create loop thread */
    _run_flag.store(true, std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_release);
    _loop_thread = new std::thread(_ActionLoop, this);
    if (_loop_thread == nullptr)
    {
        throw std::string("Couldn't run a loop thread!");
    }
}

void LoopThread::Stop()
{
    std::atomic_thread_fence(std::memory_order_release);
    _run_flag.store(false, std::memory_order_relaxed);
    
    if (_loop_thread)
    {
        _loop_thread->join();
        _loop_thread.reset();
    }
}
