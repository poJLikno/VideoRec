#ifndef FRAMES_DOUBLE_BUFFER_H_
#define FRAMES_DOUBLE_BUFFER_H_

#include <windows.h>

#include "../DoubleBuffer.h"

#include "HWAccelCL.h"

extern "C"
{
#include <libswscale/swscale.h>
}

class FramesDblBuff : public DoubleBuffer
{
protected:
    AVFrame *_frames[2] = { 0 };
    HWAccelCL _hw_accel_cl;

    uint8_t *_staged_buffer = nullptr;

    virtual void _OnWrite(const uint8_t &index) override final;

public:
    FramesDblBuff(const std::pair<int, int> &src_size, const std::pair<int, int> &dst_size);
    FramesDblBuff(const FramesDblBuff &) = delete;
    virtual ~FramesDblBuff() override;

    virtual void SetBuffer(uint8_t *staged_buffer) final;

    /* Need lock */
    virtual AVFrame *GetFrame() final;
};

#endif /* FRAMES_DOUBLE_BUFFER_H_ */
