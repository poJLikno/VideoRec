#include "FileNameGenerator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <thread>

FileNameGenerator::FileNameGenerator()
    : _thread_id(std::hash<std::thread::id>{}(std::this_thread::get_id()))
{
    int user_name_length = 128;
    _user_name = new char[user_name_length + 1] { 0 };
    GetUserNameA(_user_name, (unsigned long *) &user_name_length);
}

const SmtObj<char[]> &FileNameGenerator::CreateFileName()
{
    SYSTEMTIME system_time = { 0 };
    GetLocalTime(&system_time);

    std::string file_name(
        "C:\\Users\\" + std::string(_user_name)
        + "\\Videos\\Recording" + std::to_string(_thread_id) + "_"
        + std::to_string(system_time.wDay) + "-"
        + std::to_string(system_time.wMonth) + "-"
        + std::to_string(system_time.wYear) + "_"
        + std::to_string(system_time.wHour) + "-"
        + std::to_string(system_time.wMinute) + "-"
        + std::to_string(system_time.wSecond)
        + ".mp4");

    int file_name_length = static_cast<int>(file_name.length()) + 1;
    _file_name = new char[file_name_length] { 0 };
    snprintf(_file_name, file_name_length, "%s", file_name.c_str());

    return _file_name;
}
