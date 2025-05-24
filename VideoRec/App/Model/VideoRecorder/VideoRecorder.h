#ifndef VIDEO_RECORDER_H_
#define VIDEO_RECORDER_H_

#include "../../SmtObj.h"

#include "ScreenCapture/ScreenCapture.h"
#include "FileMP4/FileMP4.h"
#include "StableTimer/StableTimer.h"

class VideoRecorder
{
private:
    SmtObj<ScreenCapture> _screen;
    SmtObj<FileMP4> _file;

    SmtObj<StableTimer> _screen_capture_timer;
    SmtObj<StableTimer> _cursor_capture_timer;
    SmtObj<StableTimer> _file_writer_timer;

    const bool &_staged_client_rect_only_flag;
    const bool &_staged_optimization_flag;
    const bool &_staged_capture_cursor_flag;

    bool _client_rect_only_flag = false;
    bool _optimization_flag = false;
    bool _capture_cursor_flag = false;

    static void _ScreenCaptureLoop(void *this_class);
    static void _CursorCaptureLoop(void *this_class);
    static void _FileWriterLoop(void *this_class);

public:
    VideoRecorder(const bool &client_rect_only_flag, /*const bool &preview_flag,*/ const bool &optimization_flag, const bool &capture_cursor_flag);
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

    SmtObj<BitmapsDblBuff> &GetPreview();
};

#endif
