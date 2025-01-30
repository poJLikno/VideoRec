#ifndef DOUBLE_BUFFER_H_
#define DOUBLE_BUFFER_H_

#include <mutex>
#include "HWAccelCL.h"

extern "C"
{
#include <libswresample/swresample.h>
}

class DoubleBuffer
{
private:
    std::mutex _mutexs[2] = {};
    int _lock_index = 0;
    int _newest_index = 1;

    AVFrame **_frames = nullptr;

    HWAccelCL _hw_accel_cl;

public:
    DoubleBuffer(const int &src_width, const int &src_height, const int &dst_width, const int &dst_height);
    DoubleBuffer(const DoubleBuffer &) = delete;
    ~DoubleBuffer();

    void LockFrame();
    void UnlockFrame();

    AVFrame *GetFrame();
    void WriteFrame(uint8_t *src_buffer);
};

#endif /* DOUBLE_BUFFER_H_ */
