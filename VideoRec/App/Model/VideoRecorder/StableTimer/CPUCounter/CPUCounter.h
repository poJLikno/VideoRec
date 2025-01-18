#ifndef CPU_COUNTER_H_
#define CPU_COUNTER_H_

#include <windows.h>

class CPUCounter
{
public:
    typedef long long Ticks;
    enum Scale
    {
        Seconds = 1,
        Milliseconds = 1000,
        Microseconds = 1000000,
        Nanoseconds = 1000000000
    };

private:
    LARGE_INTEGER _buf = { 0 };

    static Ticks _Frequency;

public:
    CPUCounter();
    ~CPUCounter() = default;

    Ticks Now();

    static long double TicksToTime(const Ticks &ticks, const Scale &prescale);

    static Ticks TimeToTicks(const long double &time, const Scale &prescale);
};

#endif/* CPU_COUNTER_H_ */
