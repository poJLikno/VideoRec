#ifndef VIDEO_RECORDER_H_
#define VIDEO_RECORDER_H_

#include "../../SmtObj.h"

#include "ScreenCapture/ScreenCapture.h"
#include "AudioCapture/AudioCapture.h"
#include "FileMP4/FileMP4.h"
#include "LoopThread/LoopThread.h"

class VideoRecorder
{
private:
    SmtObj<ScreenCapture> _screen;
    SmtObj<AudioCapture> _audio;
    SmtObj<FileMP4> _file;

    SmtObj<LoopThread> _screen_capture_loop;
    SmtObj<LoopThread> _cursor_capture_loop;
    SmtObj<LoopThread> _file_writer;

    const bool &_staged_client_rect_only_flag;
    const bool &_staged_optimization_flag;
    const bool &_staged_capture_cursor_flag;

    bool _client_rect_only_flag = false;
    bool _optimization_flag = false;
    bool _capture_cursor_flag = false;


    static void _ScreenCaptureLoop(void *ptr);
    static void _CursorCaptureLoop(void *ptr);
    static void _FileWriterLoop(void *ptr);

public:
    VideoRecorder(const bool &client_rect_only_flag, const bool &optimization_flag, const bool &capture_cursor_flag);
    VideoRecorder(const VideoRecorder &) = delete;
    ~VideoRecorder();

    void ApplyFlags();

    void StartRecording(const char *file_name, const int &fps);
    void StopRecording();

    void SetNewSource(const char *wnd_name, const int &dst_width = -1, const int &dst_height = -1);

    void StartIdleMode();
    void StopIdleMode();

    const int &GetSrcWidth();
    const int &GetSrcHeight();

    BitmapsDblBuff *GetPreview();
};

#endif
