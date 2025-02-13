#include "VideoRecorder.h"

#include <string>

void VideoRecorder::_ScreenCaptureLoop(void *this_class)
{
    ((VideoRecorder *)this_class)->_screen->TakeShot();
}

void VideoRecorder::_CursorCaptureLoop(void *this_class)
{
    ((VideoRecorder *)this_class)->_screen->CaptureCursorState();
}

void VideoRecorder::_FileWriterLoop(void *this_class)
{
    ((VideoRecorder *)this_class)->_screen->GetFramesDblBuff()->Lock();
    ((VideoRecorder *)this_class)->_file->WriteFrame(((VideoRecorder *)this_class)->_screen->GetFramesDblBuff()->GetFrame());
    ((VideoRecorder *)this_class)->_screen->GetFramesDblBuff()->Unlock();
}

VideoRecorder::VideoRecorder(const bool &client_rect_only_flag, const bool &preview_flag, const bool &optimization_flag, const bool &capture_cursor_flag)
    : _client_rect_only_flag(client_rect_only_flag), _preview_flag(preview_flag), _optimization_flag(optimization_flag), _capture_cursor_flag(capture_cursor_flag)
{}

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
    if (_cursor_capture_timer)
    {
        if (_cursor_capture_timer->IsRunning())
        {
            _cursor_capture_timer->Stop();
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

    /* Init file writer timer and start recording */
    if (_capture_cursor_flag)
    {
        _cursor_capture_timer = new StableTimer(fps + 5, _CursorCaptureLoop, this);
        _cursor_capture_timer->Start();
    }

    _screen_capture_timer = new StableTimer(fps + 5, _ScreenCaptureLoop, this);
    _screen_capture_timer->Start();

    _file_writer_timer = new StableTimer(fps, _FileWriterLoop, this);
    _file_writer_timer->Start();
}

void VideoRecorder::StopRecording()
{
    if (_file_writer_timer)
    {
        _file_writer_timer->Stop();
    }
    
    if (_file)
    {
        _file->CloseFile();
        _file.reset();
    }
    
    if (_preview_flag)
    {
        if (_capture_cursor_flag)
        {
            _cursor_capture_timer = new StableTimer(30/*Base value*/, _CursorCaptureLoop, this);
            _cursor_capture_timer->Start();
        }
        else
        {
            _cursor_capture_timer.reset();
        }

        _screen_capture_timer = new StableTimer(30/*Base value*/, _ScreenCaptureLoop, this);
        _screen_capture_timer->Start();
    }
    else
    {
        _screen_capture_timer.reset();
        _cursor_capture_timer.reset();
    }
}

void VideoRecorder::SetNewSource(const char *wnd_name, const int &dst_width, const int &dst_height)
{
    if (_file)
    {
        throw std::string("Couldn't change captured screen!");
    }
    if (_screen_capture_timer)
    {
        if (_screen_capture_timer->IsRunning())
        {
            _screen_capture_timer->Stop();
        }
    }
    if (_cursor_capture_timer)
    {
        if (_cursor_capture_timer->IsRunning())
        {
            _cursor_capture_timer->Stop();
        }
    }

    /* Capture the screen or window */
    _screen = new ScreenCapture(wnd_name, _client_rect_only_flag, _optimization_flag, _capture_cursor_flag, dst_width, dst_height);

    /* Init screen capture timer and cursor capture timer */
    if (_preview_flag)
    {
        if (_capture_cursor_flag)
        {
            _cursor_capture_timer = new StableTimer(30/*Base value*/, _CursorCaptureLoop, this);
            _cursor_capture_timer->Start();
        }

        _screen_capture_timer = new StableTimer(30/*Base value*/, _ScreenCaptureLoop, this);
        _screen_capture_timer->Start();
    }
}

const int &VideoRecorder::GetSrcWidth()
{
    if (_screen)
    {
        return (const int &)_screen->GetSrcWidth();
    }

    //return (const int &)0;
    throw std::string("Couldn't get src width!");
}

const int &VideoRecorder::GetSrcHeight()
{
    if (_screen)
    {
        return (const int &)_screen->GetSrcHeight();
    }

    //return (const int &)0;
    throw std::string("Couldn't get src height!");
}

SmtObj<BitmapsDblBuff> &VideoRecorder::GetPreview()
{
    if (_screen)
    {
        return _screen->GetBitmapsDblBuff();
    }

    //return nullptr;
    throw std::string("Couldn't get preview!");
}
