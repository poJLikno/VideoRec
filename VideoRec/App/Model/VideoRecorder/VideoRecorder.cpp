#include "VideoRecorder.h"

#include <string>

void VideoRecorder::_ScreenCaptureLoop(void *this_class)
{
    ((VideoRecorder *)this_class)->_screen->TakeShot();
}

void VideoRecorder::_FileWriterLoop(void *this_class)
{
    ((VideoRecorder *)this_class)->_screen->GetFramesBuffer()->LockFrame();
    ((VideoRecorder *)this_class)->_file->WriteFrame(((VideoRecorder *)this_class)->_screen->GetFramesBuffer()->GetFrame());
    ((VideoRecorder *)this_class)->_screen->GetFramesBuffer()->UnlockFrame();
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
    /* Stop & release modules */
    _file_writer_timer.reset();
    _file.reset();
    _screen_capture_timer.reset();
}

void VideoRecorder::SetNewSource(const int &dst_width, const int &dst_height)
{
    if (_file)
    {
        throw std::string("Couldn't change captured screen!");
    }

    /* Capture the screen or window */
    _screen = new ScreenCapture(dst_width, dst_height);
}

const int &VideoRecorder::GetSrcWidth()
{
    if (_screen)
    {
        return (const int &)_screen->GetSrcWidth();
    }

    return (const int &)0;
}

const int &VideoRecorder::GetSrcHeight()
{
    if (_screen)
    {
        return (const int &)_screen->GetSrcHeight();
    }

    return (const int &)0;
}
