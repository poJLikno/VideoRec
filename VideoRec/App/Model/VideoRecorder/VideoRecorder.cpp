#include "VideoRecorder.h"

#include <string>

void VideoRecorder::_ScreenCaptureLoop(void *ptr)
{
    VideoRecorder *this_class = ((VideoRecorder *)ptr);

    this_class->_screen->CaptureScreenFrame();
}

void VideoRecorder::_CursorCaptureLoop(void *ptr)
{
    VideoRecorder *this_class = ((VideoRecorder *)ptr);

    this_class->_screen->CaptureCursorState();
}

void VideoRecorder::_FileWriterLoop(void *ptr)
{
    VideoRecorder *this_class = ((VideoRecorder *)ptr);

    if (av_compare_ts(this_class->_file->GetVideoStream()->next_pts, this_class->_file->GetVideoStream()->codec_context->time_base,
        this_class->_file->GetAudioStream()->next_pts, this_class->_file->GetAudioStream()->codec_context->time_base) <= 0)
    {
        this_class->_screen->GetFramesDblBuff()->Lock();
        this_class->_file->WriteVideoFrame(this_class->_screen->GetFramesDblBuff()->GetFrame());
        this_class->_screen->GetFramesDblBuff()->Unlock();
    }
    else
    {
        this_class->_audio->CaptureAudioSamples();
    
        if (this_class->_audio->GetSamplesSglBuff()->GetNoDataFlag())
        {
            return;
        }
    
        this_class->_file->WriteAudioFrame(this_class->_audio->GetSamplesSglBuff()->GetFrame());
    }
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
        _cursor_capture_timer = new StableTimer(10 * fps, _CursorCaptureLoop, this);
        _cursor_capture_timer->Start();
    }

    _screen_capture_timer = new StableTimer(10 * fps, _ScreenCaptureLoop, this);
    _screen_capture_timer->Start();

    /* Init audio capture */
    _audio = new AudioCapture();

    /* Delay for first frame creation */
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    /* Init a file and file writer */
    _file = new FileMP4(file_name, fps, _screen->GetDstWidth(), _screen->GetDstHeight(), _audio->GetWaveFormat());
    _file_writer = new LoopThread(_FileWriterLoop, this);

    /* Start video & audio file writer */
    _audio->StartPlayAndCaptureStreams();
    _file_writer->Start();
}

void VideoRecorder::StopRecording()
{
    if (_file_writer)
    {
        _file_writer.reset();
    }
    
    if (_file)
    {
        _file.reset();
    }

    if (_audio)
    {
        _audio.reset();
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
