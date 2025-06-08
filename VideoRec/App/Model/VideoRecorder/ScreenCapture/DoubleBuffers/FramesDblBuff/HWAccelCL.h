#ifndef HW_ACCCEL_CL_H_
#define HW_ACCCEL_CL_H_

#include <stdint.h>

#define CL_TARGET_OPENCL_VERSION 300

extern "C"
{
#include <CL/opencl.h>
}

#include "../../../../../../SmtObj.h"

/* #pragma comment(lib, "OpenCL.lib") */

class HWAccelCL
{
private:
    cl_int _result = 0;

    cl_platform_id _platform_id = nullptr;
    cl_device_id _device_id = nullptr;

    cl_context _context = nullptr;
    cl_command_queue _command_queue = nullptr;

    SmtObj<char[]> _kernel_code;
    cl_program _program = nullptr;
    cl_kernel _kernel = nullptr;

    int _src_width = 0;
    int _src_height = 0;
    int _dst_width = 0;
    int _dst_height = 0;

    size_t _src_rgb_buffer_size = 0ull;
    size_t _dst_y_buffer_size = 0ull;
    size_t _dst_uv_buffer_size = 0ull;

    cl_mem _cl_src_rgb = nullptr;
    cl_mem _cl_dst_y = nullptr;
    cl_mem _cl_dst_u = nullptr;
    cl_mem _cl_dst_v = nullptr;

    size_t _GetKernelCode(const char *file_name, SmtObj<char[]> *kernel_code);

public:
    HWAccelCL(const char *file_name, const char *kernel_name, const int &dst_width, const int &dst_height, const int &src_width, const int &src_height);
    HWAccelCL(const HWAccelCL &) = delete;

    ~HWAccelCL();

    void Run(uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *src_rgb);

    static void ShowOpenCLDevices();
};

#endif /* HW_ACCCEL_CL_H_ */
