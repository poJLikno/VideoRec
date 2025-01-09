__kernel void image_resample(__global unsigned char* dst_y, __global unsigned char* dst_u, __global unsigned char* dst_v, int dst_width, int dst_height, __global unsigned char* src_rgb, int src_width, int src_height)
{
	unsigned long i = get_global_id(0);

    unsigned long x = i % dst_width;
    unsigned long y = i / dst_width;

    float rel_pix_pos_h = (float)(y) / (float)(dst_height - 1) * (src_height - 1);
    int rel_h = (int)floor(rel_pix_pos_h);
    if (rel_h < 0)
    {
        rel_h = 0;
    }
    else if (rel_h >= src_height - 1)
    {
        rel_h = src_height - 2;
    }

    float pix_dist_h = rel_pix_pos_h - rel_h;

    float rel_pix_pos_w = (float)(x) / (float)(dst_width - 1) * (src_width - 1);
    int rel_w = (int)floor(rel_pix_pos_w);
    if (rel_w < 0)
    {
        rel_w = 0;
    }
    else if (rel_w >= src_width - 1)
    {
        rel_w = src_width - 2;
    }

    float pix_dist_w = rel_pix_pos_w - rel_w;

    float d1 = (1 - pix_dist_w) * (1 - pix_dist_h);
    float d2 = pix_dist_w * (1 - pix_dist_h);
    float d3 = pix_dist_w * pix_dist_h;
    float d4 = (1 - pix_dist_w) * pix_dist_h;

    unsigned char p1b = src_rgb[4 * (rel_h * src_width + rel_w)];
    unsigned char p1g = src_rgb[4 * (rel_h * src_width + rel_w) + 1];
    unsigned char p1r = src_rgb[4 * (rel_h * src_width + rel_w) + 2];

    unsigned char p2b = src_rgb[4 * (rel_h * src_width + rel_w + 1)];
    unsigned char p2g = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 1];
    unsigned char p2r = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 2];

    unsigned char p3b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1)];
    unsigned char p3g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 1];
    unsigned char p3r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 2];

    unsigned char p4b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w)];
    unsigned char p4g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 1];
    unsigned char p4r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 2];

    unsigned char b = p1b * d1 + p2b * d2 + p3b * d3 + p4b * d4;
    unsigned char g = p1g * d1 + p2g * d2 + p3g * d3 + p4g * d4;
    unsigned char r = p1r * d1 + p2r * d2 + p3r * d3 + p4r * d4;

    b = b * 219 / 255 + 16;
    g = g * 219 / 255 + 16;
    r = r * 219 / 255 + 16;

    dst_y[y * dst_width + x] = ((66 * r + 129 * g + 25 * b) >> 8);
    if (!(x % 2) && !(y % 2))
    {
        dst_u[(y * dst_width / 2 + x) / 2] = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
        dst_v[(y * dst_width / 2 + x) / 2] = ((112 * r + -94 * g + -18 * b) >> 8) + 128;
    }
}

__kernel void image_resample_no_resize(__global unsigned char* dst_y, __global unsigned char* dst_u, __global unsigned char* dst_v, int width, int height, __global unsigned char* src_rgb)
{
	unsigned long i = get_global_id(0);

    unsigned long x = i % width;
    unsigned long y = i / width;

    unsigned char b = src_rgb[4 * (y * width + x)] * 219 / 255 + 16;
    unsigned char g = src_rgb[4 * (y * width + x) + 1] * 219 / 255 + 16;
    unsigned char r = src_rgb[4 * (y * width + x) + 2] * 219 / 255 + 16;

    dst_y[y * width + x] = ((66 * r + 129 * g + 25 * b) >> 8);
    if (!(x % 2) && !(y % 2))
    {
        dst_u[(y * width / 2 + x) / 2] = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
        dst_v[(y * width / 2 + x) / 2] = ((112 * r + -94 * g + -18 * b) >> 8) + 128;
    }
}
