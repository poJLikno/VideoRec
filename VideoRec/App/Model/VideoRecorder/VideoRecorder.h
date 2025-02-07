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
    SmtObj<StableTimer> _file_writer_timer;

    bool &_preview_flag;
    bool &_client_rect_only_flag;
    bool &_optimization_flag;

    static void _ScreenCaptureLoop(void *this_class);
    static void _FileWriterLoop(void *this_class);

public:
    VideoRecorder(bool &client_rect_only_flag, bool &preview_flag, bool &optimization_flag);
    VideoRecorder(const VideoRecorder &) = delete;
    ~VideoRecorder();

    void StartRecording(const char *file_name, const int &fps);
    void StopRecording();

    void SetNewSource(const char *wnd_name, const int &dst_width = -1, const int &dst_height = -1);

    const int &GetSrcWidth();
    const int &GetSrcHeight();

    HDC GetPreviewContext();
};

#endif
