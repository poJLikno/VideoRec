#ifndef STABLE_TIMER_H_
#define STABLE_TIMER_H_

#include <stdint.h>
#include <chrono>
#include <thread>

#include "../../SmtObj.h"

#include "CPUCounter/CPUCounter.h"

class StableTimer
{
private:
    /* CPU Counter */
    CPUCounter _counter = CPUCounter();

    /* Loop thread */
    SmtObj<std::thread> _timer_loop;
    bool _run_flag = false;

    /* Input value */
    uint16_t _fps = 0;
    void (*_loop_action)(void *) = nullptr;
    void *_action_parameters = nullptr;

    /* Timepoints */
    CPUCounter::Ticks _dst_timepoint = 0ll;
    CPUCounter::Ticks _check_period_start_timepoint = 0.0l;

    /* Loop periods */
    long double _real_loop_period = 0.0l;
    long double _dst_loop_period = 0.0l;
    long double _loop_period_ctrl = 0.0l;

    /* PID - loop methods */
    static void _TimerLoop(StableTimer *integral_timer);

public:
    StableTimer(const uint16_t &dst_fps, void (*function)(void *) = nullptr, void *func_parameters = nullptr);
    StableTimer(const StableTimer &) = delete;
    ~StableTimer();

    void Start();
    void Stop();

    const bool &IsRunning();

    //void SetFps(const uint16_t &fps);
};

#endif /* STABLE_TIMER_H_ */
