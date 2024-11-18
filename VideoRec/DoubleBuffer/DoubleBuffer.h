#ifndef DOUBLE_BUFFER_H_
#define DOUBLE_BUFFER_H_

#include "FormatTransform.h"

class DoubleBuffer
{
private:
    AVFrame **_frames = nullptr;
    uint8_t _curr_safe_frame_idx = 0;

    FormatTransform _fmt_transform;

public:
    DoubleBuffer(uint8_t **src_buffer, const int &src_width, const int &src_height, const int &dst_width, const int &dst_height);
    ~DoubleBuffer();

    AVFrame *GetFrame();/* Not used */
    void WriteFrame();
};

#endif /* DOUBLE_BUFFER_H_ */
