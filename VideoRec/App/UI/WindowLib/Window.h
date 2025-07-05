#ifndef WINDOWLIB_WINDOW_H_
#define WINDOWLIB_WINDOW_H_

#define WINDOWLIB_VERSION "v1.5"

#include "WndList.h"
#include "Button.h"
#include "RadioButton.h"
#include "CheckBox.h"
#include "ComboBox.h"
#include "Edit.h"
#include "Label.h"
#include "Menu.h"

#ifdef WINDOWLIB_NO_CONSOLE
#pragma comment(linker, "/subsystem:WINDOWS")
#endif
#pragma comment(linker, "/entry:mainCRTStartup")

#ifdef WINDOWLIB_USE_MANIFEST
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#ifndef HINST_THISCOMPONENT
extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class Window;

// Class - helper
class WindowPosSize
{
public:
    Window *wnd;
    std::pair<int, int> *pos;
    std::pair<int, int> *size;
};

class Window : public WndBase, public CallbackManager
{
protected:
    // Window variables
    std::unique_ptr<wchar_t[]> _w_class_name;

    MSG _msg = { 0 };
    bool _ctrls_dlg_msg_flag = false;

    WindowPosSize _wnd_pos_size = { this, &_pos, &_size };

    // All exist windows
    static WndList<WindowPosSize> _wnd_list;

    // The Window index in lists
    int _wnd_index = 0;

    // Window controls lists
    WndList<Button> _btns_list;
    WndList<ComboBox> _cmb_bxs_list;
    WndList<Edit> _edits_list;
    WndList<Label> _lbls_list;
    Menu *_menu = nullptr;

    // Main window procedure
    static LRESULT __stdcall _DefWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
    Window(
        const char *name,
        const std::pair<int, int> &pos = { CW_USEDEFAULT, CW_USEDEFAULT },
        const std::pair<int, int> &size = { CW_USEDEFAULT, CW_USEDEFAULT },
        const int &icon_id = -1,
        const unsigned long &wnd_style = WS_OVERLAPPEDWINDOW, WNDPROC wnd_proc = nullptr);
    virtual ~Window();

    virtual void AttachChildControl(WndBase *wnd);
    virtual void AttachMenu(Menu *menu);

    virtual WndList<Button> &GetButtonsList() final;
    virtual WndList<ComboBox> &GetComboBoxesList() final;
    virtual WndList<Edit> &GetEditsList() final;
    virtual WndList<Label> &GetLabelsList() final;
    virtual Menu &GetMenu() final;

    virtual void EnableControlsDialogMessages(const bool &flag) final;

    virtual int Run();
};

class ParentResizeCallbackParams
{
public:
    WndBase *wnd;
    std::pair<int, int> old_size;
};

#endif /* WINDOWLIB_WINDOW_H_ */
