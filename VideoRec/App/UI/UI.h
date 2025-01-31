#ifndef UI_H_
#define UI_H_

#include "../../SmtObj.h"
#include "WindowLib/Window.h"
#include "WindowLib/NormalFont.h"
#include "PreviewWindow/PreviewWindow.h"

#include "../../resource.h"

#ifndef GETTER
#define GETTER(obj) decltype(obj) &get##obj() { return obj; }
#endif

class UI
{
private:
    SmtObj<Window> _wnd;
    SmtObj<NormalFont> _normal_font;

    SmtObj<Label> _video_resolution_label;
    SmtObj<Label> _video_resolution_x_symbol_label;
    SmtObj<Label> _video_fps_label;

    SmtObj<Edit> _video_width_edit;
    SmtObj<Edit> _video_height_edit;
    SmtObj<Edit> _video_fps_edit;

    SmtObj<Button> _video_settings_apply_button;

    SmtObj<MenuPoint> _start_recording_menu_point;
    SmtObj<MenuPoint> _stop_recording_menu_point;

    SmtObj<PopupMenu> _options_menu;
    SmtObj<Menu> _wnd_menu;

public:
    UI(const char *app_name, const char *app_version);
    UI(const UI &) = delete;
    ~UI() = default;

    GETTER(_wnd)

    GETTER(_video_resolution_label)
    GETTER(_video_resolution_x_symbol_label)
    GETTER(_video_fps_label)

    GETTER(_video_width_edit)
    GETTER(_video_height_edit)
    GETTER(_video_fps_edit)

    GETTER(_video_settings_apply_button)

    GETTER(_start_recording_menu_point)
    GETTER(_stop_recording_menu_point)

    GETTER(_options_menu)
    GETTER(_wnd_menu)

};

#endif /* UI_H_ */
