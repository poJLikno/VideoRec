#include "StableTimer.h"

#include <iostream>

void StableTimer::_TimerLoop(StableTimer *stable_timer)
{
    try
    {
        /**//*
        double loop_num = 1;
        double current_fps = 0;
        double old_fps = 0;
        double min_fps = 9999;
        double max_fps = -9999;
        double avg_fps = 0;
        double fps_changes = 0;
        *//**/

        while (stable_timer->_run_flag)
        {
            if (stable_timer->_counter.Now() > stable_timer->_dst_timepoint)
            {
                if (stable_timer->_loop_action)
                {
                    stable_timer->_loop_action(stable_timer->_action_parameters);
                }

                stable_timer->_real_loop_period = CPUCounter::TicksToTime(stable_timer->_counter.Now() - stable_timer->_check_period_start_timepoint, CPUCounter::Scale::Microseconds);
                stable_timer->_check_period_start_timepoint = stable_timer->_counter.Now();
                /**//*
                old_fps = current_fps;
                current_fps = 1000000.0l / stable_timer->_real_loop_period;
                if (loop_num == 150)
                {
                    loop_num = 1;
                    avg_fps = 0;
                }
                if (current_fps > max_fps || (long long)loop_num % 100 == 0) max_fps = current_fps;
                if (current_fps < min_fps || (long long)loop_num % 100 == 0) min_fps = current_fps;
                avg_fps = (avg_fps * (loop_num - 1) + current_fps) / loop_num;
                fps_changes = current_fps - old_fps;
                ++loop_num;

                std::cout << current_fps << " | min: " << min_fps << " | avg: " << avg_fps << " | max: " << max_fps << " | ";
                if (fps_changes >= 0)
                {
                    std::cout << "+" << fps_changes;// << "\n";
                }
                else
                {
                    std::cout << fps_changes;// << "\n";
                }

                std::cout << " | val: " << stable_timer->_loop_period_ctrl << "\n";
                *//**/
                /* Fix loop period */
                stable_timer->_loop_period_ctrl += stable_timer->_dst_loop_period - stable_timer->_real_loop_period;
                if (stable_timer->_loop_period_ctrl < 0.0l)
                {
                    stable_timer->_loop_period_ctrl = 0.0l;
                }
                stable_timer->_dst_timepoint = stable_timer->_counter.Now() + CPUCounter::TimeToTicks(stable_timer->_loop_period_ctrl, CPUCounter::Scale::Microseconds);
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

StableTimer::StableTimer(const uint32_t &dst_fps, void (*function)(void *), void *func_parameters)
    : _fps(dst_fps), _loop_action(function), _action_parameters(func_parameters),
    _dst_loop_period(1000000.0l / (long double)_fps)/* in microseconds */, _loop_period_ctrl(_dst_loop_period)
{}

StableTimer::~StableTimer()
{
    Stop();
}

void StableTimer::Start()
{
    if (_timer_loop)
    {
        throw std::string("StableTimer loop has been already started!");
    }

    /* Setup loop settings */
    _run_flag = true;
    _check_period_start_timepoint = _counter.Now();
    _dst_timepoint = _counter.Now() + CPUCounter::TimeToTicks(_loop_period_ctrl, CPUCounter::Scale::Microseconds);

    /* Create loop thread */
    _timer_loop = new std::thread(_TimerLoop, this);
    if (_timer_loop == nullptr)
    {
        throw std::string("Couldn't run a timer loop!");
    }
}

void StableTimer::Stop()
{
    _run_flag = false;

    if (_timer_loop)
    {
        _timer_loop->join();
        _timer_loop.reset();
    }
}
