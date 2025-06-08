# **VideoRec** #

[![WinAPI: C++](https://img.shields.io/badge/WinAPI-C++-blue)](https://learn.microsoft.com/en-us/windows/win32/api/)  [![Lib: WASAPI](https://img.shields.io/badge/Lib-WASAPI-darkred)](https://learn.microsoft.com/en-us/windows/win32/coreaudio/wasapi)  [![Lib: FFmpeg](https://img.shields.io/badge/Lib-FFmpeg-green)](https://www.ffmpeg.org/)  [![Lib: OpenCL](https://img.shields.io/badge/Lib-OpenCL-orange)](https://www.khronos.org/opencl/)  [![Lib: WindowLib](https://img.shields.io/badge/Lib-WindowLib-aqua)](https://github.com/poJLikno/WindowLib)  [![Developer: poJLikno](https://img.shields.io/badge/Developer-poJLikno-red)](https://github.com/poJLikno)  [![License: GPL v3](https://img.shields.io/badge/License-GPLv3-purple)](https://www.gnu.org/licenses/gpl-3.0)

### This app supplies opportunity to record high quality and perfomace videos of screen or windows. ###

![VideoRec_3_8_GDI-video](https://github.com/user-attachments/assets/6887111e-2536-4748-bde9-511bd05f4a0f)

## Features ##
* Video recording
* Capture windows (support layered windows)
* Capture cursor
* Preview captured video
* Capture loopback audio
* H264 codec
* MP3 codec
* MP4 output file format
* Hotkey support
* Double buffer async file write
* Dynamic audio pts
* DPI unaware

## How to use ##
1. Select ___source window___ for capture or capture desktop
2. Check/Uncheck optimization setting in __entire window__ capture mode
3. Setup settings for video (resolution, fps)
4. Choose a method of caption
5. Press "___Apply___"
6. Select "Options" -> "___Start recording___" or press hotkey (Alt + K)

## How to build from sources ##
For successful app compilation you need install OpenCL and FFmpeg libs.
Visual Studio supply vcpkg manager and this solution has vcpkg dependencies, so you need just run `vcpkg install --triplet=x64-windows` in VS powrshell to install libraries.
Now you are ready to build the app.

## Releases ##
You can get the latest release ***[right here](https://github.com/poJLikno/VideoRec/releases)***!
