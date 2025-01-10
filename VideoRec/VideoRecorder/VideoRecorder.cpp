#include "VideoRecorder.h"

#include <string>

void VideoRecorder::_ScreenCaptureLoop(void *this_class)
{
    ((VideoRecorder *)this_class)->_screen->TakeShot();
}

void VideoRecorder::_FileWriterLoop(void *this_class)
{
    ((VideoRecorder *)this_class)->_screen->FramesBuffer().LockFrame();
    ((VideoRecorder *)this_class)->_file->WriteFrame(((VideoRecorder *)this_class)->_screen->FramesBuffer().GetFrame());
    ((VideoRecorder *)this_class)->_screen->FramesBuffer().UnlockFrame();
}

VideoRecorder::~VideoRecorder()
{
    if (_file)
    {
        _file_writer_timer->Stop();

        _file->CloseFile();
    }

    if (_screen_capture_timer)
    {
        if (_screen_capture_timer->IsRunning())
        {
            _screen_capture_timer->Stop();
        }
    }
}

void VideoRecorder::StartRecording(const char *file_name, const int &fps)
{
    if (!_screen)
    {
        throw std::string("VideoRecorder doesn't have a source!");
    }
    if (_file)
    {
        throw std::string("VideoRecorder has already been recording!");
    }

    /* Create file */
    _file = new FileMP4(file_name, fps, _screen->GetDstWidth(), _screen->GetDstHeight());

    /* Init file writer timer */
    _screen_capture_timer = new StableTimer(fps + 5, _ScreenCaptureLoop, this);
    _file_writer_timer = new StableTimer(fps, _FileWriterLoop, this);

    /* Start recording */
    _screen_capture_timer->Start();
    _file_writer_timer->Start();
}

void VideoRecorder::StopRecording()
{
    if (_file_writer_timer)
    {
        _file_writer_timer->Stop();
    }

    if (_screen_capture_timer)
    {
        _screen_capture_timer->Stop();
    }

    if (_file)
    {
        _file->CloseFile();
        _file.reset();
    }
    /*_file = nullptr;*/

    /* For preview window */
    //_screen_capture_timer->SetFps(10);

}

void VideoRecorder::SetNewSource(const char *wnd_name, const int &dst_width, const int &dst_height)
{
    if (_file)
    {
        throw std::string("Couldn't change captured screen!");
    }
    else if (_screen_capture_timer)
    {
        if (_screen_capture_timer->IsRunning())
        {
            _screen_capture_timer->Stop();
        }
    }

    /* Capture the screen or window */
    _screen = new ScreenCapture(wnd_name, dst_width, dst_height);

    /* Init screen capture timer */
    _screen_capture_timer = new StableTimer(10/*Base value*/, _ScreenCaptureLoop, this);
}
