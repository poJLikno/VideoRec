#ifndef PREVIEW_WINDOW_H_
#define PREVIEW_WINDOW_H_

#include <chrono>
#include <thread>
#include <atomic>

#include "../WindowLib/Window.h"

#include "../../Model/VideoRecorder/LoopThread/LoopThread.h"
#include "../../Model/VideoRecorder/ScreenCapture/DoubleBuffers/BitmapsDblBuff/BitmapsDblBuff.h"

class PreviewWindow : public Window
{
private:
    HDC _hdc = nullptr;
    HGDIOBJ _old_obj = nullptr;

    BitmapsDblBuff *_bitmaps_dbl_buff = nullptr;

    std::unique_ptr<LoopThread> _paint_loop = nullptr;

public:
    PreviewWindow(const std::pair<int, int> &pos, const std::pair<int, int> &size);
    PreviewWindow(const PreviewWindow &) = delete;
    ~PreviewWindow();

    void SetPreview(BitmapsDblBuff *bitmaps_dbl_buff);
    void DeletePreview();
};

#endif /* PREVIEW_WINDOW_H_ */
