# **VideoRec** *(a school project)* #

[![WinAPI: C++](https://img.shields.io/badge/WinAPI-C++-blue)](https://learn.microsoft.com/ru-ru/windows/win32/api/)  [![Lib: FFmpeg](https://img.shields.io/badge/Lib-FFmpeg-green)](https://www.ffmpeg.org/)  [![Lib: OpenCL](https://img.shields.io/badge/Lib-OpenCL-orange)](https://www.khronos.org/opencl/)  [![Lib: WindowLib](https://img.shields.io/badge/Lib-WindowLib-aqua)](https://github.com/poJLikno/WindowLib)  [![Developer: poJLikno](https://img.shields.io/badge/Developer-poJLikno-red)](https://github.com/poJLikno)  [![License: GPL v3](https://img.shields.io/badge/License-GPLv3-purple)](https://www.gnu.org/licenses/gpl-3.0)

### This app supplies opportunity to record high quality and perfomace videos of screen or windows. ###

![VideoRec_3_3_GDI-video](https://github.com/user-attachments/assets/14654787-59f0-4095-a19e-0d613e31b2a2)

## Features ##
* Video recording
* Capture windows (support layered windows)
* Capture cursor
* Preview captured video
* H264 codec 
* MP4 output file format
* Hotkey support
* Double buffer async file write
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

## More information ##
You can find more information in russian language in my *[school project .docx file](https://docs.google.com/document/d/1Acz6KpHh8o7UEKdiUX9S0lwbHd2BfUYzEftklc-Ur8o/edit?usp=sharing)*.

## Releases ##
You can get the latest release ***[right here](https://github.com/poJLikno/VideoRec/releases)***!
