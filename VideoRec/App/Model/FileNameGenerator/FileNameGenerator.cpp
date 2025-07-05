#include "FileNameGenerator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <thread>
#include <lmcons.h>

#include "../../UI/WindowLib/TextUtils.h"

FileNameGenerator::FileNameGenerator()
    : _thread_id(std::hash<std::thread::id>{}(std::this_thread::get_id()))
{
    wchar_t w_user_name[UNLEN + 1] = { 0 };
    int user_name_size = sizeof(w_user_name);

#pragma warning(push)/* Suppress this warning only here */
#pragma warning(disable : 6386)
    GetUserNameW(w_user_name, (unsigned long *) &user_name_size);// return length + '\0' in user_name_size
//#pragma warning(default : 6386) set to default (enable)
#pragma warning(pop)

    _user_name = std::unique_ptr<char[]>(to_utf8(w_user_name));
}

const char *FileNameGenerator::CreateFileName()
{
    SYSTEMTIME system_time = { 0 };
    GetLocalTime(&system_time);

    std::string file_name(
        "C:\\Users\\" + std::string(_user_name.get())
        + "\\Videos\\Recording" + std::to_string(_thread_id) + "__"
        + std::to_string(system_time.wYear) + "-"
        + std::to_string(system_time.wMonth) + "-"
        + std::to_string(system_time.wDay) + "__"
        + std::to_string(system_time.wHour) + "-"
        + std::to_string(system_time.wMinute) + "-"
        + std::to_string(system_time.wSecond)
        + ".mp4");

    int file_name_length = (int)file_name.length() + 1;
    _file_name = std::make_unique<char[]>(file_name_length);
    snprintf(_file_name.get(), file_name_length, "%s", file_name.c_str());

    return _file_name.get();
}
