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
    ((VideoRecorder *)this_class)->_file->WriteVideoFrame(((VideoRecorder *)this_class)->_screen->GetFramesDblBuff()->GetFrame());
    ((VideoRecorder *)this_class)->_screen->GetFramesDblBuff()->Unlock();
}

VideoRecorder::VideoRecorder(const bool &client_rect_only_flag, const bool &optimization_flag, const bool &capture_cursor_flag)
    : _staged_client_rect_only_flag(client_rect_only_flag), _staged_optimization_flag(optimization_flag), _staged_capture_cursor_flag(capture_cursor_flag)
{
    ApplyFlags();
}

VideoRecorder::~VideoRecorder()
{
    StopRecording();
}

void VideoRecorder::ApplyFlags()
{
    /* Apply all flags (need for save conditions check) */
    _client_rect_only_flag = _staged_client_rect_only_flag;
    _optimization_flag = _staged_optimization_flag;
    _capture_cursor_flag = _staged_capture_cursor_flag;
}

void VideoRecorder::StartRecording(const char *file_name, const int &fps)
{
    /* Check requirements */
    if (!_screen)
    {
        throw std::string("VideoRecorder doesn't have a source!");
    }
    if (_file)
    {
        throw std::string("VideoRecorder has already been recording!");
    }

    /* Init screen capture timer and cursor capture timer */
    if (_capture_cursor_flag)
    {
        _cursor_capture_timer = new StableTimer(fps + 5, _CursorCaptureLoop, this);
        _cursor_capture_timer->Start();
    }

    _screen_capture_timer = new StableTimer(fps + 5, _ScreenCaptureLoop, this);
    _screen_capture_timer->Start();

    /* Delay for first frame creation */
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    /* Init a file and start recording */
    _file = new FileMP4(file_name, fps, _screen->GetDstWidth(), _screen->GetDstHeight());
    _file_writer_timer = new StableTimer(fps, _FileWriterLoop, this);
    _file_writer_timer->Start();
}

void VideoRecorder::StopRecording()
{
    /* Cleanup all timers and file */
    if (_file_writer_timer)
    {
        _file_writer_timer.reset();
    }
    
    if (_file)
    {
        _file.reset();
    }
    
    if (_screen_capture_timer)
    {
        _screen_capture_timer.reset();
    }

    if (_cursor_capture_timer)
    {
        _cursor_capture_timer.reset();
    }
}

void VideoRecorder::SetNewSource(const char *wnd_name, const int &dst_width, const int &dst_height)
{
    /* Check requirements */
    if (_file)
    {
        throw std::string("Couldn't change captured screen!");
    }

    /* Capture the screen or window */
    _screen = new ScreenCapture(wnd_name, _client_rect_only_flag, _optimization_flag, _capture_cursor_flag, dst_width, dst_height);
}

void VideoRecorder::StartIdleMode()
{
    /* Check requirements */
    if (!_screen)
    {
        throw std::string("VideoRecorder doesn't have a source!");
    }

    /* If recording, do not change fps */
    if (_file) return;

    /* Init screen capture timer and cursor capture timer */
    if (_capture_cursor_flag)
    {
        _cursor_capture_timer = new StableTimer(30/*Base value*/, _CursorCaptureLoop, this);
        _cursor_capture_timer->Start();
    }

    _screen_capture_timer = new StableTimer(30/*Base value*/, _ScreenCaptureLoop, this);
    _screen_capture_timer->Start();
}

void VideoRecorder::StopIdleMode()
{
    /* If recording, do not stop the timers */
    if (_file) return;

    /* Stop the screen capture & cursor capture timers */
    if (_screen_capture_timer)
    {
        _screen_capture_timer.reset();
    }

    if (_cursor_capture_timer)
    {
        _cursor_capture_timer.reset();
    }
}

const int &VideoRecorder::GetSrcWidth()
{
    if (_screen)
    {
        return (const int &)_screen->GetSrcWidth();
    }

    throw std::string("Couldn't get src width!");
}

const int &VideoRecorder::GetSrcHeight()
{
    if (_screen)
    {
        return (const int &)_screen->GetSrcHeight();
    }

    throw std::string("Couldn't get src height!");
}

SmtObj<BitmapsDblBuff> &VideoRecorder::GetPreview()
{
    if (_screen)
    {
        return _screen->GetBitmapsDblBuff();
    }

    throw std::string("Couldn't get preview!");
}
