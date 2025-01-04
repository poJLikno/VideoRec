#include <iostream>
#include <string>

#include <windows.h>

#include "VideoRecorder/VideoRecorder.h"

/*
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")

#pragma comment(lib, "OpenCL.lib")
*/

#include <fstream>

int main(int argc, const char **argv)
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    try
    {
        std::cout << "Aloha!\n";

        VideoRecorder rec;
        rec.SetNewSource(nullptr, 1920, 1080);
        //rec.SetNewSource(nullptr, 1440, 900);
        //rec.SetNewSource(nullptr, 800, 600);
        //rec.SetNewSource(nullptr, 2880, 1800);
        rec.StartRecording("test_file.mp4", 60);
        while (GetAsyncKeyState('L') >= 0);
        rec.StopRecording();

        std::cout << "Bye!\n";

    }
    catch (const std::string &error)
    {
        MessageBoxA(NULL, error.c_str(), "Error!", MB_OK);
    }
    
    return 0;
}

static void ShowOpenCLDevices()
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
    for (int i = 0; i < platforms_num; ++i)
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
        for (int j = 0; j < devices_num; ++j)
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

int main_old(int argc, const char **argv)
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    try
    {
        std::cout << "Aloha!\n";

        cl_int ret = 0;

        cl_platform_id platform_id = nullptr;
        cl_device_id device_id = nullptr;

        cl_context context = nullptr;
        cl_command_queue command_queue = nullptr;

        ShowOpenCLDevices();

        ret = clGetPlatformIDs(1, &platform_id, nullptr);
        if (ret)
        {
            throw std::string("Error #1!");
        }

        /* получить доступные устройства */
        ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, nullptr);
        if (ret)
        {
            throw std::string("Error #2!");
        }

        /* создать контекст */
        context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
        if (ret)
        {
            throw std::string("Error #3!");
        }

        /* создаем команду */
        //command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
        command_queue = clCreateCommandQueueWithProperties(context, device_id, nullptr, &ret);
        if (ret)
        {
            throw std::string("Error #4!");
        }

        /* Create Kernel */
        cl_program program = nullptr;
        cl_kernel kernel = nullptr;

        size_t file_size = 0ull;
        char* gpu_code = nullptr;

        /* Read code from file */
        std::ifstream file("gpu_prog.cl");
        if (file.is_open())
        {
            /* Get file size */
            file.seekg(0, std::ios_base::end);
            file_size = file.tellg();
            file.seekg(0, std::ios_base::beg);

            /* Load code from file to buffer */
            gpu_code = new char[file_size + 1ull] { 0 };
            file.read(gpu_code, file_size);

            /* Close file */
            file.close();
        }
        else
        {
            throw std::string("Couldn't open file!");
        }

        /* Check file output */
        std::cout << gpu_code << "\n";

        /* создать бинарник из кода программы */
        program = clCreateProgramWithSource(context, 1, (const char **)&gpu_code, (const size_t *)&file_size, &ret);
        if (ret)
        {
            throw std::string("Error #5!");
        }

        /* скомпилировать программу */
        ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
        if (ret)
        {
            throw std::string("Error #6!");
        }

        /* создать кернел */
        kernel = clCreateKernel(program, "test", &ret);
        if (ret)
        {
            throw std::string("Error #7!");
        }

        /* Create buffer */
        int memLength = 16;
        cl_int *mem = new cl_int[memLength]
        { 0, 0, 255, 0,
          0, 255, 0, 0,
          255, 0, 0, 0,
          255, 255, 255, 0 };/* BGR0 */

        /* создать буфер */
        cl_mem memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, memLength * sizeof(cl_int), NULL, &ret);
        if (ret)
        {
            throw std::string("Error #8!");
        }

        /* записать данные в буфер */
        ret = clEnqueueWriteBuffer(command_queue, memobj, CL_TRUE, 0, memLength * sizeof(cl_int), mem, 0, NULL, NULL);
        if (ret)
        {
            throw std::string("Error #9!");
        }

        clFinish(command_queue);

        /* устанавливаем параметр */
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);
        if (ret)
        {
            throw std::string("Error #10!");
        }

        /* Run GPU code */
        size_t global_work_size = 16ull;

        /* выполнить кернел */
        ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, (const size_t *) &global_work_size, NULL, 0, NULL, NULL);

        clFinish(command_queue);

        /* считать данные из буфера */
        ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0, memLength * sizeof(cl_int), mem, 0, NULL, NULL);

        for (int i = 0; i < memLength; ++i)
        {
            std::cout << mem[i] << "\n";
        }

        /* Clean up */
        delete[] mem;
        mem = nullptr;

        delete[] gpu_code;
        gpu_code = nullptr;

        clReleaseMemObject(memobj);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(command_queue);
        clReleaseContext(context);
        clReleaseDevice(device_id);
    }
    catch (const std::string &error)
    {
        std::cout << error << "\n";
    }

    return 0;
}
