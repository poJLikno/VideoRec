#include "LoopThread.h"

#include <string>
#include <windows.h>

#include "../../../UI/WindowLib/TextUtils.h"

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
        std::unique_ptr<wchar_t[]> w_error(to_utf16(error.c_str()));
        MessageBoxW(NULL, w_error.get(), L"Error", MB_OK);
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
    _loop_thread = std::unique_ptr<std::thread>(new std::thread(_ActionLoop, this));
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
