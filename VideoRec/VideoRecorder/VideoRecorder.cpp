#include "VideoRecorder.h"

#include <string>

void VideoRecorder::_ScreenCaptureLoop(void *this_class)
{
    ((VideoRecorder *)this_class)->_screen.Ref().TakeShot();
}

void VideoRecorder::_FileWriterLoop(void *this_class)
{
    ((VideoRecorder *)this_class)->_screen.Ref().FramesBuffer().LockFrame();
    ((VideoRecorder *)this_class)->_file.Ref().WriteFrame(((VideoRecorder *)this_class)->_screen.Ref().FramesBuffer().GetFrame());
    ((VideoRecorder *)this_class)->_screen.Ref().FramesBuffer().UnlockFrame();
}

VideoRecorder::~VideoRecorder()
{
    if (_file.Ptr())
    {
        _file_writer_timer.Ref().Stop();

        _file.Ref().CloseFile();
    }

    if (_screen_capture_timer.Ref().IsRunning())
    {
        _screen_capture_timer.Ref().Stop();
    }
}

void VideoRecorder::StartRecording(const char *file_name, const int &fps)
{
    if (!_screen.Ptr())
    {
        throw std::string("VideoRecorder doesn't have a source!");
    }
    if (_file.Ptr())
    {
        throw std::string("VideoRecorder has already been recording!");
    }

    /* Create file */
    _file = new FileMP4(file_name, fps, _screen.Ref().GetDstWidth(), _screen.Ref().GetDstHeight());

    /* Init file writer timer */
    _screen_capture_timer = new StableTimer(fps + 5, _ScreenCaptureLoop, this);
    _file_writer_timer = new StableTimer(fps, _FileWriterLoop, this);

    /* Start recording */
    _screen_capture_timer.Ref().Start();
    _file_writer_timer.Ref().Start();
}

void VideoRecorder::StopRecording()
{
    if (!_file.Ptr())
    {
        throw std::string("VideoRecorder has already stopped!");
    }

    _file_writer_timer.Ref().Stop();
    _screen_capture_timer.Ref().Stop();

    _file.Ref().CloseFile();
    _file = nullptr;

    /* For preview window */
    //_screen_capture_timer->SetFps(10);

}

void VideoRecorder::SetNewSource(const char *wnd_name, const int &dst_width, const int &dst_height)
{
    if (_file.Ptr())
    {
        throw std::string("Couldn't change captured screen!");
    }
    else if (_screen_capture_timer.Ptr())
    {
        if (_screen_capture_timer.Ref().IsRunning())
        {
            _screen_capture_timer.Ref().Stop();
        }
    }

    /* Capture the screen or window */
    _screen = new ScreenCapture(wnd_name, dst_width, dst_height);

    /* Init screen capture timer */
    _screen_capture_timer = new StableTimer(10/*Base value*/, _ScreenCaptureLoop, this);
}
