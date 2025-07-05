#ifndef WINDOWLIB_WND_BASE_H_
#define WINDOWLIB_WND_BASE_H_

#include <iostream>
#include <utility>
#include <string>
#include <windows.h>

#include "TextUtils.h"

class WndBase {
protected:
    HWND _hwnd = nullptr;
    std::pair<int, int> _pos = { 0, 0 };
    std::pair<int, int> _size = { 0, 0 };
    bool _is_visible = true;
    bool _input_state = true;

    WndBase *_parent_wnd = nullptr;

    WndBase(const std::pair<int, int> &pos, const std::pair<int, int> &size)
        : _pos(pos), _size(size) {};

public:
    virtual ~WndBase();

    virtual void ShowWnd(const bool &flag) final;
    virtual LRESULT SendMsg(const UINT &msg, const WPARAM &wparam, const LPARAM &lparam) final;

    virtual HWND GetHwnd() final;

    virtual bool GetInputState() const final;
    virtual void SetInputState(const bool &flag) final;

    /* Need free after use (delete[] ...); max_symbols_count include null-symbol */
    virtual char *GetWndText(const int &max_symbols_count) final;
    virtual void SetWndText(const char *label) final;

    /* Problem section */
    virtual LONG_PTR GetWndStyle() final;
    virtual void SetWndStyle(const LONG_PTR &params) final;

    virtual WndBase *GetWndParent() final;
    virtual void SetWndParent(WndBase *wnd);/* Non final cause can be problems */
    /* --------------- */

    virtual void UpdateWnd() final;

    virtual std::pair<int, int> GetWndPos() final;
    virtual void SetWndPos(const std::pair<int, int> &pos) final;

    virtual std::pair<int, int> GetWndSize() final;
    virtual void SetWndSize(const std::pair<int, int> &size) final;

    virtual int GetWndHeaderSize() final;
    virtual int GetWndSideBorderSize() final;
    virtual int GetWndBottomBorderSize() final;

    virtual std::pair<int, int> GetWndClientPos() final;
    virtual void SetWndClientPos(const std::pair<int, int> &pos) final;

    virtual std::pair<int, int> GetWndClientSize() final;
    virtual void SetWndClientSize(const std::pair<int, int> &size) final;

    virtual void SetWndOrderZ(HWND hwnd) final;
};

#endif /* WINDOWLIB_WND_BASE_H_ */
