#ifndef DOUBLE_BUFFER_H_
#define DOUBLE_BUFFER_H_

#include "HWAccelCL.h"

extern "C"
{
#include <libswresample/swresample.h>
}

class DoubleBuffer
{
private:
    AVFrame **_frames = nullptr;
    uint8_t _curr_safe_frame_idx = 0;

    HWAccelCL _hw_accel_cl;

    bool _frame_is_locked = false;
    bool _has_new_frame = false;

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
