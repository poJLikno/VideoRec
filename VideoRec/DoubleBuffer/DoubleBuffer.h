#ifndef DOUBLE_BUFFER_H_
#define DOUBLE_BUFFER_H_

#include "FormatTransform.h"

class DoubleBuffer
{
private:
    AVFrame **_frames = nullptr;
    uint8_t _curr_safe_frame_idx = 0;

    FormatTransform _fmt_transform;

    bool _frame_is_locked = false;
    bool _has_new_frame = false;

public:
    DoubleBuffer(uint8_t *src_buffer, const int &width, const int &height);
    ~DoubleBuffer();

    void LockFrame();
    void UnlockFrame();

    AVFrame *GetFrame();
    void WriteFrame();
};

#endif /* DOUBLE_BUFFER_H_ */
