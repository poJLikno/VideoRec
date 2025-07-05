#ifndef HW_ACCCEL_CL_H_
#define HW_ACCCEL_CL_H_

#include <memory>
#include <stdint.h>

#define CL_TARGET_OPENCL_VERSION 300

extern "C"
{
#include <CL/opencl.h>
}

/* #pragma comment(lib, "OpenCL.lib") */

class HWAccelCL
{
private:
    cl_int _result = 0;

    cl_platform_id _platform_id = nullptr;
    cl_device_id _device_id = nullptr;

    cl_context _context = nullptr;
    cl_command_queue _command_queue = nullptr;

    std::unique_ptr<char[]> _kernel_code;
    cl_program _program = nullptr;
    cl_kernel _kernel = nullptr;

    const std::pair<int, int> _src_size;
    const std::pair<int, int> _dst_size;

    size_t _src_rgb_buffer_size = 0ull;
    size_t _dst_y_buffer_size = 0ull;
    size_t _dst_uv_buffer_size = 0ull;

    cl_mem _cl_src_rgb = nullptr;
    cl_mem _cl_dst_y = nullptr;
    cl_mem _cl_dst_u = nullptr;
    cl_mem _cl_dst_v = nullptr;

    size_t _GetKernelCode(const char *file_name, std::unique_ptr<char[]> &kernel_code);

public:
    HWAccelCL(const char *file_name, const char *kernel_name, const std::pair<int, int> &dst_size, const std::pair<int, int> &src_size);
    HWAccelCL(const HWAccelCL &) = delete;

    ~HWAccelCL();

    void Run(uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *src_rgb);

    static void ShowOpenCLDevices();
};

#endif /* HW_ACCCEL_CL_H_ */
