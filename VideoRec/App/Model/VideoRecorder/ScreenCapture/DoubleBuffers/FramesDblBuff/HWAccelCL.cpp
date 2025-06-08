#include "HWAccelCL.h"

#include <fstream>
#include <string>

#include <iostream>

size_t HWAccelCL::_GetKernelCode(const char *file_name, SmtObj<char[]> *kernel_code)
{
    //size_t file_size = 0ull;

    ///* Read code from file */
    //std::ifstream file(file_name);
    //if (file.is_open())
    //{
    //    /* Get file size */
    //    file.seekg(0, std::ios_base::end);
    //    file_size = file.tellg();
    //    file.seekg(0, std::ios_base::beg);

    //    /* Load code from file to buffer */
    //    *kernel_code = new char[file_size + 1ull] { 0 };
    //    file.read(*kernel_code, file_size);

    //    /* Close file */
    //    file.close();
    //}
    //else
    //{
    //    throw std::string("Couldn't open file!");
    //}

    //return file_size;

    *kernel_code = new char[]
        {
            "\
            __kernel void image_resample(__global unsigned char* dst_y, __global unsigned char* dst_u, __global unsigned char* dst_v, int dst_width, int dst_height, __global unsigned char* src_rgb, int src_width, int src_height)\
            {\
                unsigned long i = get_global_id(0);\
                \
                /* Get coordinates */\
                unsigned long x = 2 * (i % (dst_width / 2));\
                unsigned long y = 2 * (i / (dst_width / 2));\
                \
                /**/\
                /* Find pixel #1 */\
                /**/\
                \
                float rel_pix_pos_h = (float)(y) / (float)(dst_height - 1) * (src_height - 1);\
                int rel_h = (int)floor(rel_pix_pos_h);\
                if (rel_h < 0)\
                {\
                    rel_h = 0;\
                }\
                else if (rel_h >= src_height - 1)\
                {\
                    rel_h = src_height - 2;\
                }\
                \
                float pix_dist_h = rel_pix_pos_h - rel_h;\
                \
                float rel_pix_pos_w = (float)(x) / (float)(dst_width - 1) * (src_width - 1);\
                int rel_w = (int)floor(rel_pix_pos_w);\
                if (rel_w < 0)\
                {\
                    rel_w = 0;\
                }\
                else if (rel_w >= src_width - 1)\
                {\
                    rel_w = src_width - 2;\
                }\
                \
                float pix_dist_w = rel_pix_pos_w - rel_w;\
                \
                float d1 = (1 - pix_dist_w) * (1 - pix_dist_h);\
                float d2 = pix_dist_w * (1 - pix_dist_h);\
                float d3 = pix_dist_w * pix_dist_h;\
                float d4 = (1 - pix_dist_w) * pix_dist_h;\
                \
                unsigned char p1b = src_rgb[4 * (rel_h * src_width + rel_w)];\
                unsigned char p1g = src_rgb[4 * (rel_h * src_width + rel_w) + 1];\
                unsigned char p1r = src_rgb[4 * (rel_h * src_width + rel_w) + 2];\
                \
                unsigned char p2b = src_rgb[4 * (rel_h * src_width + rel_w + 1)];\
                unsigned char p2g = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 1];\
                unsigned char p2r = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 2];\
                \
                unsigned char p3b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1)];\
                unsigned char p3g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 1];\
                unsigned char p3r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 2];\
                \
                unsigned char p4b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w)];\
                unsigned char p4g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 1];\
                unsigned char p4r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 2];\
                \
                unsigned char b1 = p1b * d1 + p2b * d2 + p3b * d3 + p4b * d4;\
                unsigned char g1 = p1g * d1 + p2g * d2 + p3g * d3 + p4g * d4;\
                unsigned char r1 = p1r * d1 + p2r * d2 + p3r * d3 + p4r * d4;\
                \
                b1 = b1 * 219 / 255 + 16;\
                g1 = g1 * 219 / 255 + 16;\
                r1 = r1 * 219 / 255 + 16;\
                \
                /* Set Y */\
                dst_y[y * dst_width + x] = ((66 * r1 + 129 * g1 + 25 * b1) >> 8);\
                \
                /**/\
                /* Find pixel #2 */\
                /**/\
                \
                rel_pix_pos_h = (float)(y) / (float)(dst_height - 1) * (src_height - 1);\
                rel_h = (int)floor(rel_pix_pos_h);\
                if (rel_h < 0)\
                {\
                    rel_h = 0;\
                }\
                if (rel_h >= src_height - 1)\
                {\
                    rel_h = src_height - 2;\
                }\
                \
                pix_dist_h = rel_pix_pos_h - rel_h;\
                \
                rel_pix_pos_w = (float)(x + 1) / (float)(dst_width - 1) * (src_width - 1);\
                rel_w = (int)floor(rel_pix_pos_w);\
                if (rel_w < 0)\
                {\
                    rel_w = 0;\
                }\
                else if (rel_w >= src_width - 1)\
                {\
                    rel_w = src_width - 2;\
                }\
                \
                pix_dist_w = rel_pix_pos_w - rel_w;\
                \
                d1 = (1 - pix_dist_w) * (1 - pix_dist_h);\
                d2 = pix_dist_w * (1 - pix_dist_h);\
                d3 = pix_dist_w * pix_dist_h;\
                d4 = (1 - pix_dist_w) * pix_dist_h;\
                \
                p1b = src_rgb[4 * (rel_h * src_width + rel_w)];\
                p1g = src_rgb[4 * (rel_h * src_width + rel_w) + 1];\
                p1r = src_rgb[4 * (rel_h * src_width + rel_w) + 2];\
                \
                p2b = src_rgb[4 * (rel_h * src_width + rel_w + 1)];\
                p2g = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 1];\
                p2r = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 2];\
                \
                p3b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1)];\
                p3g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 1];\
                p3r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 2];\
                \
                p4b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w)];\
                p4g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 1];\
                p4r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 2];\
                \
                unsigned char b2 = p1b * d1 + p2b * d2 + p3b * d3 + p4b * d4;\
                unsigned char g2 = p1g * d1 + p2g * d2 + p3g * d3 + p4g * d4;\
                unsigned char r2 = p1r * d1 + p2r * d2 + p3r * d3 + p4r * d4;\
                \
                b2 = b2 * 219 / 255 + 16;\
                g2 = g2 * 219 / 255 + 16;\
                r2 = r2 * 219 / 255 + 16;\
                \
                /* Set Y */\
                dst_y[y * dst_width + x + 1] = ((66 * r2 + 129 * g2 + 25 * b2) >> 8);\
                \
                /**/\
                /* Find pixel #3 */\
                /**/\
                \
                rel_pix_pos_h = (float)(y + 1) / (float)(dst_height - 1) * (src_height - 1);\
                rel_h = (int)floor(rel_pix_pos_h);\
                if (rel_h < 0)\
                {\
                    rel_h = 0;\
                }\
                if (rel_h >= src_height - 1)\
                {\
                    rel_h = src_height - 2;\
                }\
                \
                pix_dist_h = rel_pix_pos_h - rel_h;\
                \
                rel_pix_pos_w = (float)(x + 1) / (float)(dst_width - 1) * (src_width - 1);\
                rel_w = (int)floor(rel_pix_pos_w);\
                if (rel_w < 0)\
                {\
                    rel_w = 0;\
                }\
                else if (rel_w >= src_width - 1)\
                {\
                    rel_w = src_width - 2;\
                }\
                \
                pix_dist_w = rel_pix_pos_w - rel_w;\
                \
                d1 = (1 - pix_dist_w) * (1 - pix_dist_h);\
                d2 = pix_dist_w * (1 - pix_dist_h);\
                d3 = pix_dist_w * pix_dist_h;\
                d4 = (1 - pix_dist_w) * pix_dist_h;\
                \
                p1b = src_rgb[4 * (rel_h * src_width + rel_w)];\
                p1g = src_rgb[4 * (rel_h * src_width + rel_w) + 1];\
                p1r = src_rgb[4 * (rel_h * src_width + rel_w) + 2];\
                \
                p2b = src_rgb[4 * (rel_h * src_width + rel_w + 1)];\
                p2g = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 1];\
                p2r = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 2];\
                \
                p3b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1)];\
                p3g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 1];\
                p3r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 2];\
                \
                p4b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w)];\
                p4g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 1];\
                p4r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 2];\
                \
                unsigned char b3 = p1b * d1 + p2b * d2 + p3b * d3 + p4b * d4;\
                unsigned char g3 = p1g * d1 + p2g * d2 + p3g * d3 + p4g * d4;\
                unsigned char r3 = p1r * d1 + p2r * d2 + p3r * d3 + p4r * d4;\
                \
                b3 = b3 * 219 / 255 + 16;\
                g3 = g3 * 219 / 255 + 16;\
                r3 = r3 * 219 / 255 + 16;\
                \
                /* Set Y */\
                dst_y[(y + 1) * dst_width + x + 1] = ((66 * r3 + 129 * g3 + 25 * b3) >> 8);\
                \
                /**/\
                /* Find pixel #4 */\
                /**/\
                \
                rel_pix_pos_h = (float)(y + 1) / (float)(dst_height - 1) * (src_height - 1);\
                rel_h = (int)floor(rel_pix_pos_h);\
                if (rel_h < 0)\
                {\
                    rel_h = 0;\
                }\
                if (rel_h >= src_height - 1)\
                {\
                    rel_h = src_height - 2;\
                }\
                \
                pix_dist_h = rel_pix_pos_h - rel_h;\
                \
                rel_pix_pos_w = (float)(x) / (float)(dst_width - 1) * (src_width - 1);\
                rel_w = (int)floor(rel_pix_pos_w);\
                if (rel_w < 0)\
                {\
                    rel_w = 0;\
                }\
                else if (rel_w >= src_width - 1)\
                {\
                    rel_w = src_width - 2;\
                }\
                \
                pix_dist_w = rel_pix_pos_w - rel_w;\
                \
                d1 = (1 - pix_dist_w) * (1 - pix_dist_h);\
                d2 = pix_dist_w * (1 - pix_dist_h);\
                d3 = pix_dist_w * pix_dist_h;\
                d4 = (1 - pix_dist_w) * pix_dist_h;\
                \
                p1b = src_rgb[4 * (rel_h * src_width + rel_w)];\
                p1g = src_rgb[4 * (rel_h * src_width + rel_w) + 1];\
                p1r = src_rgb[4 * (rel_h * src_width + rel_w) + 2];\
                \
                p2b = src_rgb[4 * (rel_h * src_width + rel_w + 1)];\
                p2g = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 1];\
                p2r = src_rgb[4 * (rel_h * src_width + rel_w + 1) + 2];\
                \
                p3b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1)];\
                p3g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 1];\
                p3r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w + 1) + 2];\
                \
                p4b = src_rgb[4 * ((rel_h + 1) * src_width + rel_w)];\
                p4g = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 1];\
                p4r = src_rgb[4 * ((rel_h + 1) * src_width + rel_w) + 2];\
                \
                unsigned char b4 = p1b * d1 + p2b * d2 + p3b * d3 + p4b * d4;\
                unsigned char g4 = p1g * d1 + p2g * d2 + p3g * d3 + p4g * d4;\
                unsigned char r4 = p1r * d1 + p2r * d2 + p3r * d3 + p4r * d4;\
                \
                b4 = b4 * 219 / 255 + 16;\
                g4 = g4 * 219 / 255 + 16;\
                r4 = r4 * 219 / 255 + 16;\
                \
                /* Set Y */\
                dst_y[(y + 1) * dst_width + x] = ((66 * r4 + 129 * g4 + 25 * b4) >> 8);\
                \
                /* Avg color */\
                unsigned char b = ((b1 + b2 + b3 + b4) >> 2);\
                unsigned char g = ((g1 + g2 + g3 + g4) >> 2);\
                unsigned char r = ((r1 + r2 + r3 + r4) >> 2);\
                \
                /* Set UV for 4x4 pixels square */\
                dst_u[(y * dst_width / 2 + x) / 2] = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;\
                dst_v[(y * dst_width / 2 + x) / 2] = ((112 * r + -94 * g + -18 * b) >> 8) + 128;\
            }\
            \
            __kernel void image_resample_no_resize(__global unsigned char *dst_y, __global unsigned char *dst_u, __global unsigned char *dst_v, int width, int height, __global unsigned char *src_rgb)\
            {\
                unsigned long i = get_global_id(0);\
                \
                /* Get coordinates */\
                unsigned long x = 2 * (i % (width / 2));\
                unsigned long y = 2 * (i / (width / 2));\
                \
                /**/\
                /* Find pixel #1 */\
                /**/\
                \
                unsigned char b1 = src_rgb[4 * (y * width + x)] * 219 / 255 + 16;\
                unsigned char g1 = src_rgb[4 * (y * width + x) + 1] * 219 / 255 + 16;\
                unsigned char r1 = src_rgb[4 * (y * width + x) + 2] * 219 / 255 + 16;\
                \
                /* Set Y */\
                dst_y[y * width + x] = ((66 * r1 + 129 * g1 + 25 * b1) >> 8);\
                \
                \
                /**/\
                /* Find pixel #2 */\
                /**/\
                \
                unsigned char b2 = src_rgb[4 * (y * width + x + 1)] * 219 / 255 + 16;\
                unsigned char g2 = src_rgb[4 * (y * width + x + 1) + 1] * 219 / 255 + 16;\
                unsigned char r2 = src_rgb[4 * (y * width + x + 1) + 2] * 219 / 255 + 16;\
                \
                /* Set Y */\
                dst_y[y * width + x + 1] = ((66 * r2 + 129 * g2 + 25 * b2) >> 8);\
                \
                /**/\
                /* Find pixel #3 */\
                /**/\
                \
                unsigned char b3 = src_rgb[4 * ((y + 1) * width + x + 1)] * 219 / 255 + 16;\
                unsigned char g3 = src_rgb[4 * ((y + 1) * width + x + 1) + 1] * 219 / 255 + 16;\
                unsigned char r3 = src_rgb[4 * ((y + 1) * width + x + 1) + 2] * 219 / 255 + 16;\
                \
                /* Set Y */\
                dst_y[(y + 1) * width + x + 1] = ((66 * r3 + 129 * g3 + 25 * b3) >> 8);\
                \
                /**/\
                /* Find pixel #4 */\
                /**/\
                \
                unsigned char b4 = src_rgb[4 * ((y + 1) * width + x)] * 219 / 255 + 16;\
                unsigned char g4 = src_rgb[4 * ((y + 1) * width + x) + 1] * 219 / 255 + 16;\
                unsigned char r4 = src_rgb[4 * ((y + 1) * width + x) + 2] * 219 / 255 + 16;\
                \
                /* Set Y */\
                dst_y[(y + 1) * width + x] = ((66 * r4 + 129 * g4 + 25 * b4) >> 8);\
                \
                /* Avg color */\
                unsigned char b = ((b1 + b2 + b3 + b4) >> 2);\
                unsigned char g = ((g1 + g2 + g3 + g4) >> 2);\
                unsigned char r = ((r1 + r2 + r3 + r4) >> 2);\
                \
                /* Set UV for 4x4 pixels square */\
                dst_u[(y * width / 2 + x) / 2] = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;\
                dst_v[(y * width / 2 + x) / 2] = ((112 * r + -94 * g + -18 * b) >> 8) + 128;\
            }\
            "
        };

    return strlen(*kernel_code) + 1;
}

HWAccelCL::HWAccelCL(const char *file_name, const char *kernel_name, const int &dst_width, const int &dst_height, const int &src_width, const int &src_height)
    : _src_width(src_width), _src_height(src_height), _dst_width(dst_width), _dst_height(dst_height),
    _src_rgb_buffer_size(src_width * src_height * 4), _dst_y_buffer_size(dst_width * dst_height), _dst_uv_buffer_size(dst_width * dst_height / 4)
{
    /* Get a default OpenCL platform */
    _result = clGetPlatformIDs(1, &_platform_id, nullptr);
    if (_result)
    {
        throw std::string("Couldn't get a platform!");
    }

    /* Get a default OpenCL device */
    _result = clGetDeviceIDs(_platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &_device_id, nullptr);
    if (_result)
    {
        throw std::string("Couldn't get a device!");
    }

    /* Create the context */
    _context = clCreateContext(NULL, 1, &_device_id, NULL, NULL, &_result);
    if (_result)
    {
        throw std::string("Couldn't create the context!");
    }

    /* Create commands queue */
    _command_queue = clCreateCommandQueueWithProperties(_context, _device_id, nullptr, &_result);
    if (_result)
    {
        throw std::string("Couldn't create the commands queue!");
    }

    /* Get kernel code */
    size_t code_length = _GetKernelCode(file_name, &_kernel_code);
    if (!code_length)
    {
        throw std::string("Couldn't read the kernel code!");
    }
    /* Check file output */
    /*std::cout << _kernel_code << "\n";*/

    /* Create the program */
    _program = clCreateProgramWithSource(_context, 1, (const char **)&_kernel_code, (const size_t *)&code_length, &_result);
    if (_result)
    {
        throw std::string("Couldn't create the program!");
    }

    /* Compile the program */
    _result = clBuildProgram(_program, 1, &_device_id, NULL, NULL, NULL);
    if (_result)
    {
        throw std::string("Couldn't compile the program!");
    }

    /* Create the kernel */
    _kernel = clCreateKernel(_program, (dst_width == src_width && dst_height == src_height ? std::string(kernel_name + std::string("_no_resize")).c_str() : kernel_name), &_result);
    if (_result)
    {
        throw std::string("Couldn't create the kernel!");
    }

    /* Create CL buffers for YUV & RGB */
    _cl_src_rgb = clCreateBuffer(_context, CL_MEM_READ_WRITE, _src_rgb_buffer_size * sizeof(cl_uchar), NULL, &_result);
    if (_result)
    {
        throw std::string("Couldn't create the CL src RGB buffer!");
    }
    _cl_dst_y = clCreateBuffer(_context, CL_MEM_READ_WRITE, _dst_y_buffer_size * sizeof(cl_uchar), NULL, &_result);
    if (_result)
    {
        throw std::string("Couldn't create the CL dst Y buffer!");
    }
    _cl_dst_u = clCreateBuffer(_context, CL_MEM_READ_WRITE, _dst_uv_buffer_size * sizeof(cl_uchar), NULL, &_result);
    if (_result)
    {
        throw std::string("Couldn't create the CL dst U buffer!");
    }
    _cl_dst_v = clCreateBuffer(_context, CL_MEM_READ_WRITE, _dst_uv_buffer_size * sizeof(cl_uchar), NULL, &_result);
    if (_result)
    {
        throw std::string("Couldn't create the CL dst V buffer!");
    }

    /* Set kernel arguments */
    _result = clSetKernelArg(_kernel, 0, sizeof(cl_mem), (void *)&_cl_dst_y);
    if (_result)
    {
        throw std::string("Couldn't pass arg #0 to kernel!");
    }
    _result = clSetKernelArg(_kernel, 1, sizeof(cl_mem), (void *)&_cl_dst_u);
    if (_result)
    {
        throw std::string("Couldn't pass arg #1 to kernel!");
    }
    _result = clSetKernelArg(_kernel, 2, sizeof(cl_mem), (void *)&_cl_dst_v);
    if (_result)
    {
        throw std::string("Couldn't pass arg #2 to kernel!");
    }
    _result = clSetKernelArg(_kernel, 3, sizeof(int), (void *)&_dst_width);
    if (_result)
    {
        throw std::string("Couldn't pass arg #3 to kernel!");
    }
    _result = clSetKernelArg(_kernel, 4, sizeof(int), (void *)&_dst_height);
    if (_result)
    {
        throw std::string("Couldn't pass arg #4 to kernel!");
    }

    _result = clSetKernelArg(_kernel, 5, sizeof(cl_mem), (void *)&_cl_src_rgb);
    if (_result)
    {
        throw std::string("Couldn't pass arg #5 to kernel!");
    }

    if (dst_width != src_width || dst_height != src_height)
    {
        _result = clSetKernelArg(_kernel, 6, sizeof(int), (void *)&_src_width);
        if (_result)
        {
            throw std::string("Couldn't pass arg #6 to kernel!");
        }
        _result = clSetKernelArg(_kernel, 7, sizeof(int), (void *)&_src_height);
        if (_result)
        {
            throw std::string("Couldn't pass arg #7 to kernel!");
        }
    }
}

HWAccelCL::~HWAccelCL()
{
    /* Clean up */
    clReleaseMemObject(_cl_src_rgb);
    clReleaseMemObject(_cl_dst_y);
    clReleaseMemObject(_cl_dst_u);
    clReleaseMemObject(_cl_dst_v);

    _cl_src_rgb = nullptr;
    _cl_dst_y = nullptr;
    _cl_dst_u = nullptr;
    _cl_dst_v = nullptr;

    clReleaseKernel(_kernel);
    clReleaseProgram(_program);
    clReleaseCommandQueue(_command_queue);
    clReleaseContext(_context);
    clReleaseDevice(_device_id);

    _kernel = nullptr;
    _program = nullptr;
    _command_queue = nullptr;
    _context = nullptr;
    _device_id = nullptr;

    _platform_id = nullptr;
}

void HWAccelCL::Run(uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *src_rgb)
{
    /* Write data to buffer */
    _result = clEnqueueWriteBuffer(_command_queue, _cl_src_rgb, CL_TRUE, 0, _src_rgb_buffer_size * sizeof(cl_uchar), src_rgb, 0, NULL, NULL);
    if (_result)
    {
        throw std::string("Couldn't write src_rgb to buffer! : " + std::to_string(_result));
    }

    /* Run the kernel */
    _result = clEnqueueNDRangeKernel(_command_queue, _kernel, 1, NULL, (const size_t *)&_dst_uv_buffer_size, NULL, 0, NULL, NULL);
    if (_result)
    {
        throw std::string("Couldn't run the kernel!");
    }

    /* Wait until end kernel */
    clFinish(_command_queue);

    /* Get YUV data */
    _result = clEnqueueReadBuffer(_command_queue, _cl_dst_y, CL_FALSE, 0, _dst_y_buffer_size * sizeof(cl_uchar), y, 0, NULL, NULL);
    if (_result)
    {
        throw std::string("Couldn't read dst Y from buffer! : " + std::to_string(_result));
    }
    _result = clEnqueueReadBuffer(_command_queue, _cl_dst_u, CL_FALSE, 0, _dst_uv_buffer_size * sizeof(cl_uchar), u, 0, NULL, NULL);
    if (_result)
    {
        throw std::string("Couldn't read dst U from buffer! : " + std::to_string(_result));
    }
    _result = clEnqueueReadBuffer(_command_queue, _cl_dst_v, CL_FALSE, 0, _dst_uv_buffer_size * sizeof(cl_uchar), v, 0, NULL, NULL);
    if (_result)
    {
        throw std::string("Couldn't read dst V from buffer! : " + std::to_string(_result));
    }

    /* Wait until end of data recording */
    clFinish(_command_queue);
}

void HWAccelCL::ShowOpenCLDevices()
{
    cl_int ret = 0;

    cl_uint platforms_num = 0u;
    cl_platform_id *platform_ids_list = nullptr;

    cl_uint devices_num = 0u;
    cl_device_id *device_ids_list = nullptr;

    std::cout << "OpenCL platform & device list:\n";

    /* Get number of platforms */
    ret = clGetPlatformIDs(0, nullptr, &platforms_num);
    if (ret)
    {
        throw std::string("Couldn't get number of platforms!");
    }

    std::cout << "Platforms number - " << platforms_num << ":\n";/* Output platform number */

    /* Get platform list */
    platform_ids_list = new cl_platform_id[platforms_num]{ nullptr };

    clGetPlatformIDs(platforms_num, platform_ids_list, nullptr);
    if (ret)
    {
        throw std::string("Couldn't get platform list!");
    }

    /* Get names of platforms */
    cl_platform_info platform_name[32] = { 0 };
    cl_device_info device_name[32] = { 0 };
    for (unsigned int i = 0; i < platforms_num; ++i)
    {
        if (clGetPlatformInfo(platform_ids_list[i], CL_PLATFORM_NAME, sizeof(platform_name), platform_name, nullptr))
        {
            throw std::string("Couldn't get platform name!");
        }
        std::cout << "[" << i << "] " << (char *)platform_name << "\n";/* Output platform name */

        /* Devices section */
        /* Get number of devices */
        ret = clGetDeviceIDs(platform_ids_list[i], CL_DEVICE_TYPE_DEFAULT, 0, nullptr, &devices_num);
        if (ret)
        {
            throw std::string("Couldn't get number of devices!");
        }

        std::cout << "\tDevices number - " << devices_num << ":\n";/* Output devices number */

        /* Get device list */
        device_ids_list = new cl_device_id[devices_num]{ nullptr };

        ret = clGetDeviceIDs(platform_ids_list[i], CL_DEVICE_TYPE_DEFAULT, devices_num, device_ids_list, nullptr);
        if (ret)
        {
            throw std::string("Couldn't get device list!");
        }

        /* Get names of devices */
        for (unsigned int j = 0; j < devices_num; ++j)
        {
            if (clGetDeviceInfo(device_ids_list[j], CL_DEVICE_NAME, sizeof(device_name), device_name, nullptr))
            {
                throw std::string("Couldn't get device name!");
            }
            std::cout << "\t[" << j << "] " << (char *)device_name << "\n";/* Output device name */
            memset(device_name, 0, sizeof(device_name));

            clReleaseDevice(device_ids_list[j]);
        }

        delete[] device_ids_list;
        device_ids_list = nullptr;

        memset(platform_name, 0, sizeof(platform_name));
    }

    std::cout << "\n";

    delete[] platform_ids_list;
    platform_ids_list = nullptr;
}
