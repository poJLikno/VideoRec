#include "FormatTransform.h"

#include <string>

FormatTransform::FormatTransform(
    uint8_t **src_buffer,
    const int &src_width,
    const int &src_height,
    const int &dst_width,
    const int &dst_height)
    : _src_buffer_linesize(src_width * 4)
{
    /* Create converter context */
    _fmt_transform_ctx = sws_getContext(
        src_width, src_height, AV_PIX_FMT_BGR0,
        dst_width, dst_height, AV_PIX_FMT_YUV420P, SWS_BILINEAR,
        nullptr, nullptr, nullptr);
    if (!_fmt_transform_ctx)
    {
        throw std::string("Couldn't create converter context!");
    }

    /* Set source buffer */
    _src[0] = *src_buffer;
}

FormatTransform::~FormatTransform()
{
    /* Release FormatTransform context */
    sws_freeContext(_fmt_transform_ctx);
    _fmt_transform_ctx = nullptr;
}

void FormatTransform::Transform(AVFrame *frame)
{
    /* Convert frame */
    if (sws_scale(_fmt_transform_ctx, _src, &_src_buffer_linesize, 0, frame->height, frame->data, frame->linesize) < frame->height)
    {
        throw std::string("Couldn't convert frame!");
    }
}
