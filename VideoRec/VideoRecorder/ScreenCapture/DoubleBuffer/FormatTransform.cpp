#include "FormatTransform.h"

#include <string>

FormatTransform::FormatTransform(
    uint8_t *src_buffer,
    const int &src_width,
    const int &src_height,
    const int &dst_width,
    const int &dst_height)
    : _src_buffer_linesize(src_width * 4), _src_height(src_height)
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
    _src[0] = src_buffer;
}

FormatTransform::~FormatTransform()
{
    /* Release FormatTransform context */
    sws_freeContext(_fmt_transform_ctx);
    _fmt_transform_ctx = nullptr;
}

static void Resample(uint8_t *src, int src_w, int src_h, uint8_t *dst, int dst_w, int dst_h)
{
    int i, j;
    int h, w;
    float t;
    float u;
    float tmp;
    float d1, d2, d3, d4;

    /* Окрестные пикселы */
    uint8_t p1b = 0u;
    uint8_t p1g = 0u;
    uint8_t p1r = 0u;

    uint8_t p2b = 0u;
    uint8_t p2g = 0u;
    uint8_t p2r = 0u;

    uint8_t p3b = 0u;
    uint8_t p3g = 0u;
    uint8_t p3r = 0u;

    uint8_t p4b = 0u;
    uint8_t p4g = 0u;
    uint8_t p4r = 0u;

    uint8_t red, green, blue;

    for (j = 0; j < dst_h; j++) {
        tmp = (float)(j) / (float)(dst_h - 1) * (src_h - 1);
        h = (int)floor(tmp);
        if (h < 0) {
            h = 0;
        }
        else {
            if (h >= src_h - 1) {
                h = src_h - 2;
            }
        }
        u = tmp - h;

        for (i = 0; i < dst_w; i++) {

            tmp = (float)(i) / (float)(dst_w - 1) * (src_w - 1);
            w = (int)floor(tmp);
            if (w < 0) {
                w = 0;
            }
            else {
                if (w >= src_w - 1) {
                    w = src_w - 2;
                }
            }
            t = tmp - w;

            /* Коэффициенты */
            d1 = (1 - t) * (1 - u);
            d2 = t * (1 - u);
            d3 = t * u;
            d4 = (1 - t) * u;

            /* Окрестные пиксели: a[i][j] */
            p1b = src[4 * (h * src_w + w)];
            p1g = src[4 * (h * src_w + w) + 1];
            p1r = src[4 * (h * src_w + w) + 2];

            p2b = src[4 * (h * src_w + w + 1)];
            p2g = src[4 * (h * src_w + w + 1) + 1];
            p2r = src[4 * (h * src_w + w + 1) + 2];

            p3b = src[4 * ((h + 1) * src_w + w + 1)];
            p3g = src[4 * ((h + 1) * src_w + w + 1) + 1];
            p3r = src[4 * ((h + 1) * src_w + w + 1) + 2];

            p4b = src[4 * ((h + 1) * src_w + w)];
            p4g = src[4 * ((h + 1) * src_w + w) + 1];
            p4r = src[4 * ((h + 1) * src_w + w) + 2];

            /* Компоненты */
            blue = p1b * d1 + p2b * d2 + p3b * d3 + p4b * d4;
            green = p1g * d1 + p2g * d2 + p3g * d3 + p4g * d4;
            red = p1r * d1 + p2r * d2 + p3r * d3 + p4r * d4;

            /* Новый пиксел из R G B  */
            dst[4 * (j * dst_w + i)] = blue;
            dst[4 * (j * dst_w + i) + 1] = green;
            dst[4 * (j * dst_w + i) + 2] = red;
        }
    }
}

static void Bitmap2Yuv420p_calc2(uint8_t *dst_y, uint8_t *dst_u, uint8_t *dst_v, uint8_t *rgb, size_t width, size_t height)
{
    size_t image_size = width * height;
    //size_t upos = image_size;
    size_t upos = 0;
    //size_t vpos = upos + upos / 4;
    size_t vpos = 0;
    size_t i = 0;

    for (size_t line = 0; line < height; ++line)
    {
        if (!(line % 2))
        {
            for (size_t x = 0; x < width; x += 2)
            {
                uint8_t b = rgb[4 * i] * 219 / 255 + 16;
                uint8_t g = rgb[4 * i + 1] * 219 / 255 + 16;
                uint8_t r = rgb[4 * i + 2] * 219 / 255 + 16;

                /* Kr = 76;
                   Kg = 150;
                   Kb = 30;
                */

                dst_y[i++] = ((66 * r + 129 * g + 25 * b) >> 8);

                dst_u[upos++] = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
                dst_v[vpos++] = ((112 * r + -94 * g + -18 * b) >> 8) + 128;

                b = rgb[4 * i] * 219 / 255 + 16;
                g = rgb[4 * i + 1] * 219 / 255 + 16;
                r = rgb[4 * i + 2] * 219 / 255 + 16;

                dst_y[i++] = ((76 * r + 150 * g + 30 * b) >> 8);
            }
        }
        else
        {
            for (size_t x = 0; x < width; x += 1)
            {
                uint8_t b = rgb[4 * i] * 219 / 255 + 16;
                uint8_t g = rgb[4 * i + 1] * 219 / 255 + 16;
                uint8_t r = rgb[4 * i + 2] * 219 / 255 + 16;

                dst_y[i++] = ((76 * r + 150 * g + 30 * b) >> 8);
            }
        }
    }
}

void FormatTransform::Transform(AVFrame *frame)
{
    /* Convert frame */
    /*memset(_src[0], (uint8_t)0, (size_t)2880 * 1800 * 4);

    _src[0][0] = 0;
    _src[0][1] = 0;
    _src[0][2] = 255;
    _src[0][3] = 0;

    _src[0][4] = 0;
    _src[0][5] = 255;
    _src[0][6] = 0;
    _src[0][7] = 0;

    _src[0][8] = 255;
    _src[0][9] = 0;
    _src[0][10] = 0;
    _src[0][11] = 0;

    _src[0][12] = 255;
    _src[0][13] = 255;
    _src[0][14] = 255;
    _src[0][15] = 0;*/

    if (sws_scale(_fmt_transform_ctx, _src, &_src_buffer_linesize, 0, _src_height, frame->data, frame->linesize) < frame->height)
    {
        throw std::string("Couldn't convert frame!");
    }

    /*uint8_t *dst = new uint8_t[frame->width * frame->height * 4];

    Resample(_src[0], _src_buffer_linesize / 4, _src_height, dst, frame->width, frame->height);
    uint8_t *old_src = _src[0];
    _src[0] = dst;
    Bitmap2Yuv420p_calc2(frame->data[0], frame->data[1], frame->data[2], _src[0], frame->width, frame->height);
    _src[0] = old_src;

    delete[] dst;
    dst = nullptr;*/
}
