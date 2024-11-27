#include "IntegralTimer.h"

#include <iostream>/**/

void IntegralTimer::_TimerLoop(IntegralTimer *integral_timer)
{
    while (integral_timer->_run_flag)
    {
        if (integral_timer->_counter.Now() > integral_timer->_dst_timepoint)
        {
            if (integral_timer->_loop_action)
            {
                integral_timer->_loop_action(integral_timer->_action_parameters);
            }

            integral_timer->_real_loop_period = CPUCounter::TicksToTime(integral_timer->_counter.Now() - integral_timer->_check_period_start_timepoint, CPUCounter::Scale::Microseconds);
            integral_timer->_check_period_start_timepoint = integral_timer->_counter.Now();
            /**/
            static double loop_num = 1;
            static double current_fps = 0;
            static double old_fps = 0;
            static double min_fps = 9999;
            static double max_fps = -9999;
            static double avg_fps = 0;
            static double fps_changes = 0;

            old_fps = current_fps;
            current_fps = 1000000.0l / integral_timer->_real_loop_period;
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
                std::cout << "+" << fps_changes << "\n";
            }
            else
            {
                std::cout << fps_changes << "\n";
            }
            /**/
            /* Fix loop period */
            integral_timer->_loop_period_ctrl += integral_timer->_dst_loop_period - integral_timer->_real_loop_period;
            integral_timer->_dst_timepoint = integral_timer->_counter.Now() + CPUCounter::TimeToTicks(integral_timer->_loop_period_ctrl, CPUCounter::Scale::Microseconds);

        }
    }
    integral_timer->_run_flag = true;
}

IntegralTimer::IntegralTimer(const uint16_t &dst_fps, void (*function)(void *), void *func_parameters)
    : _fps(dst_fps), _loop_action(function), _action_parameters(func_parameters),
    _dst_loop_period(1000000.0l / _fps)/* in microseconds */, _loop_period_ctrl(_dst_loop_period)
{

}

IntegralTimer::~IntegralTimer()
{
    if (_timer_loop)
    {
        Stop();
        _run_flag = false;
    }
}

void IntegralTimer::Start()
{
    if (_timer_loop || _run_flag == true)
    {
        throw std::string("PIDTimer loop has been already started!");
    }

    /* Setup loop settings */
    _run_flag = true;
    _check_period_start_timepoint = _counter.Now();
    _dst_timepoint = _counter.Now() + CPUCounter::TimeToTicks(_loop_period_ctrl, CPUCounter::Scale::Microseconds);

    /* Create loop thread */
    _timer_loop = new std::thread(_TimerLoop, this);
    _timer_loop->detach();
}

void IntegralTimer::Stop()
{
    if (!_timer_loop || _run_flag == false)
    {
        throw std::string("PIDTimer are not running now!");
    }

    _run_flag = false;
    while (!_run_flag);
    _run_flag = false;

    delete _timer_loop;
    _timer_loop = nullptr;
}
