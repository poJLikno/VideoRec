#ifndef FORMAT_TRANSFORM_H_
#define FORMAT_TRANSFORM_H_

#include <stdint.h>

extern "C"
{
    #include <libswscale/swscale.h>
}

class FormatTransform
{
private:
    SwsContext *_fmt_transform_ctx = nullptr;

    uint8_t *_src[4] = { nullptr };
    const int _src_buffer_linesize;

public:
    FormatTransform(uint8_t **src_buffer, const int &src_width, const int &src_height, const int &dst_width, const int &dst_height);
    ~FormatTransform();

    void Transform(AVFrame *frame);
};

#endif /* FORMAT_TRANSFORM_H_ */
