#include "HWAccelCL.h"

#include <fstream>
#include <string>

#include <iostream>

size_t HWAccelCL::_GetKernelCode(const char *file_name, SmtObj<char[]> *kernel_code)
{
    size_t file_size = 0ull;

    /* Read code from file */
    std::ifstream file(file_name);
    if (file.is_open())
    {
        /* Get file size */
        file.seekg(0, std::ios_base::end);
        file_size = file.tellg();
        file.seekg(0, std::ios_base::beg);

        /* Load code from file to buffer */
        *kernel_code = new char[file_size + 1ull] { 0 };
        file.read(*kernel_code, file_size);

        /* Close file */
        file.close();
    }
    else
    {
        throw std::string("Couldn't open file!");
    }

    return file_size;
}

HWAccelCL::HWAccelCL(const char *file_name, const char *kernel_name, const int &dst_width, const int &dst_height, uint8_t *src_rgb, const int &src_width, const int &src_height)
    : _src_rgb(src_rgb), _src_width(src_width), _src_height(src_height), _dst_width(dst_width), _dst_height(dst_height),
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
    _src_rgb = nullptr;

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

void HWAccelCL::Run(uint8_t *y, uint8_t *u, uint8_t *v)
{
    /* Write data to buffer */
    _result = clEnqueueWriteBuffer(_command_queue, _cl_src_rgb, CL_TRUE, 0, _src_rgb_buffer_size * sizeof(cl_uchar), _src_rgb, 0, NULL, NULL);
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
            std::cout << "\t[" << i << "] " << (char *)device_name << "\n";/* Output device name */
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
