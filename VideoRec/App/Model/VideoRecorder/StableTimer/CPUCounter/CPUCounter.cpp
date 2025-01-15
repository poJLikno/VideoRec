#include "CPUCounter.h"

CPUCounter::Ticks CPUCounter::_Frequency = 0ll;

CPUCounter::CPUCounter::CPUCounter()
{
    if (_Frequency) return;
    QueryPerformanceFrequency(&_buf);
    _Frequency = _buf.QuadPart;
}

CPUCounter::Ticks CPUCounter::CPUCounter::Now()
{
    QueryPerformanceCounter(&_buf);
    return _buf.QuadPart;
}

long double CPUCounter::TicksToTime(const Ticks &ticks, const Scale &prescale)
{
    return (long double)ticks * (long double)prescale / (long double)_Frequency;
}

CPUCounter::Ticks CPUCounter::TimeToTicks(const long double &time, const Scale &prescale)
{
    return static_cast<Ticks>(time * (long double)_Frequency / (long double)prescale);
}
