#include "FormatTransform.h"

#include <string>

FormatTransform::FormatTransform(
    uint8_t *src_buffer,
    const int &width,
    const int &height)
    : _src_buffer_linesize(width * 4)
{
    /* Create converter context */
    _fmt_transform_ctx = sws_getContext(
        width, height, AV_PIX_FMT_BGR0,
        width, height, AV_PIX_FMT_YUV420P, SWS_BILINEAR,
        nullptr, nullptr, nullptr);
    if (!_fmt_transform_ctx)
    {
        throw std::string("Couldn't create converter context!");
    }

    /* Set source buffer */
    _src[0] = src_buffer;
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
