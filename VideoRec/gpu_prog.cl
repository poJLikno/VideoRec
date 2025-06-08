__kernel void image_resample(__global unsigned char* dst_y, __global unsigned char* dst_u, __global unsigned char* dst_v, int dst_width, int dst_height, __global unsigned char* src_rgb, int src_width, int src_height)
{
    unsigned long i = get_global_id(0);

    /* Get coordinates */
    unsigned long x = 2 * (i % (dst_width / 2));
    unsigned long y = 2 * (i / (dst_width / 2));

    /**/
    /* Find pixel #1 */
    /**/

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

    unsigned char b1 = p1b * d1 + p2b * d2 + p3b * d3 + p4b * d4;
    unsigned char g1 = p1g * d1 + p2g * d2 + p3g * d3 + p4g * d4;
    unsigned char r1 = p1r * d1 + p2r * d2 + p3r * d3 + p4r * d4;

    b1 = b1 * 219 / 255 + 16;
    g1 = g1 * 219 / 255 + 16;
    r1 = r1 * 219 / 255 + 16;

    /* Set Y */
    dst_y[y * dst_width + x] = ((66 * r1 + 129 * g1 + 25 * b1) >> 8);

    /**/
    /* Find pixel #2 */
    /**/

    rel_pix_pos_h = (float)(y) / (float)(dst_height - 1) * (src_height - 1);
    rel_h = (int)floor(rel_pix_pos_h);
    if (rel_h < 0)
    {
        rel_h = 0;
    }
    if (rel_h >= src_height - 1)
    {
        rel_h = src_height - 2;
    }

    pix_dist_h = rel_pix_pos_h - rel_h;

    rel_pix_pos_w = (float)(x + 1) / (float)(dst_width - 1) * (src_width - 1);
    rel_w = (int)floor(rel_pix_pos_w);
    if (rel_w < 0)
    {
        rel_w = 0;
    }
    else if (rel_w >= src_width - 1)
    {
        rel_w = src_width - 2;
    }

    pix_dist_w = rel_pix_pos_w - rel_w;

    d1 = (1 - pix_dist_w) * (1 - pix_dist_h);
    d2 = pix_dist_w * (1 - pix_dist_h);
    d3 = pix_dist_w * pix_dist_h;
    d4 = (1 - pix_dist_w) * pix_dist_h;

    p1b = src_rgb[4 * (rel_h * src_width + rel_w)];
    p1g = src_rgb[4 * (rel_h * src_width + rel_w) + 1];
    p1r = src_rgb[4 * (rel_h * src_width + rel_w) + 2];

    p2b = src_rgb[4 * (rel_h * src_width + rel_w + 1)];
    p2g = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 1];
    p2r = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 2];

    p3b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1)];
    p3g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 1];
    p3r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 2];

    p4b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w)];
    p4g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 1];
    p4r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 2];

    unsigned char b2 = p1b * d1 + p2b * d2 + p3b * d3 + p4b * d4;
    unsigned char g2 = p1g * d1 + p2g * d2 + p3g * d3 + p4g * d4;
    unsigned char r2 = p1r * d1 + p2r * d2 + p3r * d3 + p4r * d4;

    b2 = b2 * 219 / 255 + 16;
    g2 = g2 * 219 / 255 + 16;
    r2 = r2 * 219 / 255 + 16;
    
    /* Set Y */
    dst_y[y * dst_width + x + 1] = ((66 * r2 + 129 * g2 + 25 * b2) >> 8);

    /**/
    /* Find pixel #3 */
    /**/

    rel_pix_pos_h = (float)(y + 1) / (float)(dst_height - 1) * (src_height - 1);
    rel_h = (int)floor(rel_pix_pos_h);
    if (rel_h < 0)
    {
        rel_h = 0;
    }
    if (rel_h >= src_height - 1)
    {
        rel_h = src_height - 2;
    }

    pix_dist_h = rel_pix_pos_h - rel_h;

    rel_pix_pos_w = (float)(x + 1) / (float)(dst_width - 1) * (src_width - 1);
    rel_w = (int)floor(rel_pix_pos_w);
    if (rel_w < 0)
    {
        rel_w = 0;
    }
    else if (rel_w >= src_width - 1)
    {
        rel_w = src_width - 2;
    }

    pix_dist_w = rel_pix_pos_w - rel_w;

    d1 = (1 - pix_dist_w) * (1 - pix_dist_h);
    d2 = pix_dist_w * (1 - pix_dist_h);
    d3 = pix_dist_w * pix_dist_h;
    d4 = (1 - pix_dist_w) * pix_dist_h;

    p1b = src_rgb[4 * (rel_h * src_width + rel_w)];
    p1g = src_rgb[4 * (rel_h * src_width + rel_w) + 1];
    p1r = src_rgb[4 * (rel_h * src_width + rel_w) + 2];

    p2b = src_rgb[4 * (rel_h * src_width + rel_w + 1)];
    p2g = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 1];
    p2r = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 2];

    p3b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1)];
    p3g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 1];
    p3r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 2];

    p4b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w)];
    p4g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 1];
    p4r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 2];

    unsigned char b3 = p1b * d1 + p2b * d2 + p3b * d3 + p4b * d4;
    unsigned char g3 = p1g * d1 + p2g * d2 + p3g * d3 + p4g * d4;
    unsigned char r3 = p1r * d1 + p2r * d2 + p3r * d3 + p4r * d4;

    b3 = b3 * 219 / 255 + 16;
    g3 = g3 * 219 / 255 + 16;
    r3 = r3 * 219 / 255 + 16;
    
    /* Set Y */
    dst_y[(y + 1) * dst_width + x + 1] = ((66 * r3 + 129 * g3 + 25 * b3) >> 8);

    /**/
    /* Find pixel #4 */
    /**/

    rel_pix_pos_h = (float)(y + 1) / (float)(dst_height - 1) * (src_height - 1);
    rel_h = (int)floor(rel_pix_pos_h);
    if (rel_h < 0)
    {
        rel_h = 0;
    }
    if (rel_h >= src_height - 1)
    {
        rel_h = src_height - 2;
    }

    pix_dist_h = rel_pix_pos_h - rel_h;

    rel_pix_pos_w = (float)(x) / (float)(dst_width - 1) * (src_width - 1);
    rel_w = (int)floor(rel_pix_pos_w);
    if (rel_w < 0)
    {
        rel_w = 0;
    }
    else if (rel_w >= src_width - 1)
    {
        rel_w = src_width - 2;
    }

    pix_dist_w = rel_pix_pos_w - rel_w;

    d1 = (1 - pix_dist_w) * (1 - pix_dist_h);
    d2 = pix_dist_w * (1 - pix_dist_h);
    d3 = pix_dist_w * pix_dist_h;
    d4 = (1 - pix_dist_w) * pix_dist_h;

    p1b = src_rgb[4 * (rel_h * src_width + rel_w)];
    p1g = src_rgb[4 * (rel_h * src_width + rel_w) + 1];
    p1r = src_rgb[4 * (rel_h * src_width + rel_w) + 2];

    p2b = src_rgb[4 * (rel_h * src_width + rel_w + 1)];
    p2g = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 1];
    p2r = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 2];

    p3b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1)];
    p3g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 1];
    p3r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 2];

    p4b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w)];
    p4g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 1];
    p4r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 2];

    unsigned char b4 = p1b * d1 + p2b * d2 + p3b * d3 + p4b * d4;
    unsigned char g4 = p1g * d1 + p2g * d2 + p3g * d3 + p4g * d4;
    unsigned char r4 = p1r * d1 + p2r * d2 + p3r * d3 + p4r * d4;

    b4 = b4 * 219 / 255 + 16;
    g4 = g4 * 219 / 255 + 16;
    r4 = r4 * 219 / 255 + 16;
    
    /* Set Y */
    dst_y[(y + 1) * dst_width + x] = ((66 * r4 + 129 * g4 + 25 * b4) >> 8);

    /* Avg color */
    unsigned char b = ((b1 + b2 + b3 + b4) >> 2);
    unsigned char g = ((g1 + g2 + g3 + g4) >> 2);
    unsigned char r = ((r1 + r2 + r3 + r4) >> 2);

    /* Set UV for 4x4 pixels square */
    dst_u[(y * dst_width / 2 + x) / 2] = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
    dst_v[(y * dst_width / 2 + x) / 2] = ((112 * r + -94 * g + -18 * b) >> 8) + 128;
}

__kernel void image_resample_no_resize(__global unsigned char* dst_y, __global unsigned char* dst_u, __global unsigned char* dst_v, int width, int height, __global unsigned char* src_rgb)
{
    unsigned long i = get_global_id(0);

    /* Get coordinates */
    unsigned long x = 2 * (i % (width / 2));
    unsigned long y = 2 * (i / (width / 2));

    /**/
    /* Find pixel #1 */
    /**/

    unsigned char b1 = src_rgb[4 * (y * width + x)] * 219 / 255 + 16;
    unsigned char g1 = src_rgb[4 * (y * width + x) + 1] * 219 / 255 + 16;
    unsigned char r1 = src_rgb[4 * (y * width + x) + 2] * 219 / 255 + 16;

    /* Set Y */
    dst_y[y * width + x] = ((66 * r1 + 129 * g1 + 25 * b1) >> 8);


    /**/
    /* Find pixel #2 */
    /**/

    unsigned char b2 = src_rgb[4 * (y * width + x + 1)] * 219 / 255 + 16;
    unsigned char g2 = src_rgb[4 * (y * width + x + 1) + 1] * 219 / 255 + 16;
    unsigned char r2 = src_rgb[4 * (y * width + x + 1) + 2] * 219 / 255 + 16;
    
    /* Set Y */
    dst_y[y * width + x + 1] = ((66 * r2 + 129 * g2 + 25 * b2) >> 8);

    /**/
    /* Find pixel #3 */
    /**/

    unsigned char b3 = src_rgb[4 * ((y + 1) * width + x + 1)] * 219 / 255 + 16;
    unsigned char g3 = src_rgb[4 * ((y + 1) * width + x + 1) + 1] * 219 / 255 + 16;
    unsigned char r3 = src_rgb[4 * ((y + 1) * width + x + 1) + 2] * 219 / 255 + 16;

    /* Set Y */
    dst_y[(y + 1) * width + x + 1] = ((66 * r3 + 129 * g3 + 25 * b3) >> 8);

    /**/
    /* Find pixel #4 */
    /**/

    unsigned char b4 = src_rgb[4 * ((y + 1) * width + x)] * 219 / 255 + 16;
    unsigned char g4 = src_rgb[4 * ((y + 1) * width + x) + 1] * 219 / 255 + 16;
    unsigned char r4 = src_rgb[4 * ((y + 1) * width + x) + 2] * 219 / 255 + 16;
    
    /* Set Y */
    dst_y[(y + 1) * width + x] = ((66 * r4 + 129 * g4 + 25 * b4) >> 8);

    /* Avg color */
    unsigned char b = ((b1 + b2 + b3 + b4) >> 2);
    unsigned char g = ((g1 + g2 + g3 + g4) >> 2);
    unsigned char r = ((r1 + r2 + r3 + r4) >> 2);

    /* Set UV for 4x4 pixels square */
    dst_u[(y * width / 2 + x) / 2] = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
    dst_v[(y * width / 2 + x) / 2] = ((112 * r + -94 * g + -18 * b) >> 8) + 128;
}
