#ifndef INTEGRAL_TIMER_H_
#define INTEGRAL_TIMER_H_

#include <stdint.h>
#include <chrono>
#include <thread>

#include "CPUCounter/CPUCounter.h"

class PIDCoeffs
{
public:
    long double p = 0.0l;
    long double i = 0.0l;
    long double d = 0.0l;
};

class IntegralTimer
{
private:
    /* CPU Counter */
    CPUCounter _counter = CPUCounter();

    /* Loop thread */
    std::thread *_timer_loop = nullptr;
    bool _run_flag = false;

    /* Input value */
    uint16_t _fps = 0;
    void (*_loop_action)(void *) = nullptr;
    void *_action_parameters = nullptr;

    /* Timepoints */
    CPUCounter::Ticks _dst_timepoint = 0.0l;
    CPUCounter::Ticks _check_period_start_timepoint = 0.0l;

    /* Loop periods */
    long double _real_loop_period = 0.0l;
    long double _dst_loop_period = 0.0l;
    long double _loop_period_ctrl = 0.0l;

    /* PID - loop methods */
    static void _TimerLoop(IntegralTimer *integral_timer);

public:
    IntegralTimer(const uint16_t &dst_fps, void (*function)(void *) = nullptr, void *func_parameters = nullptr);
    ~IntegralTimer();

    void Start();
    void Stop();
};

#endif /* INTEGRAL_TIMER_H_ */
