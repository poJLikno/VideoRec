#ifndef PREVIEW_WINDOW_H_
#define PREVIEW_WINDOW_H_

#include <chrono>
#include <thread>

#include "../../SmtObj.h"

#include "../WindowLib/WndBase.h"
#include "../WindowLib/WndList.h"

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

    HDC _src_hdc = nullptr;
    int _src_width = 0;
    int _src_height = 0;

    SmtObj<std::thread> _paint_timer;
    bool _timer_flag = true;

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    PreviewWindow(WndBase *parent_wnd, const WndPairValue &pos, const WndPairValue &size);
    PreviewWindow(const PreviewWindow &) = delete;
    ~PreviewWindow();

    void SetSrc(HDC src_hdc, const int &src_width, const int &src_height);
};

#endif /* PREVIEW_WINDOW_H_ */
