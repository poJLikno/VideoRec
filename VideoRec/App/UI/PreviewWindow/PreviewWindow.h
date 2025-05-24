#ifndef PREVIEW_WINDOW_H_
#define PREVIEW_WINDOW_H_

#include <chrono>
#include <thread>

#include "../../SmtObj.h"

#include "../WindowLib/WndBase.h"
#include "../WindowLib/WndList.h"

#include "../../Model/VideoRecorder/ScreenCapture/DoubleBuffers/BitmapsDblBuff/BitmapsDblBuff.h"

class PreviewWindow : public WndBase
{
private:
    // All exist windows
    static WndList<PreviewWindow> _wnd_list;
    // All windows pos/size variables
    static WndList<WndPairValue> _wnd_pos_list;
    static WndList<WndPairValue> _wnd_size_list;
    /* the Window index in lists */
    int _wnd_list_index = 0;

    HDC _hdc = nullptr;
    HPEN _pen = nullptr;
    HGDIOBJ _old_obj = nullptr;

    SmtObj<BitmapsDblBuff> *_bitmaps_dbl_buff = nullptr;
    bool _delete_preview_flag = false;

    SmtObj<std::thread> _paint_timer;
    bool _timer_flag = true;

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    PreviewWindow(WndBase *parent_wnd, const WndPairValue &pos, const WndPairValue &size);
    PreviewWindow(const PreviewWindow &) = delete;
    ~PreviewWindow();

    void SetPreview(SmtObj<BitmapsDblBuff> &bitmaps_dbl_buff);
    void DeletePreview();
};

#endif /* PREVIEW_WINDOW_H_ */
