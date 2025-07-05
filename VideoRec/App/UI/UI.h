#ifndef UI_H_
#define UI_H_

#include <memory>

#include "MainWindow/MainWindow.h"
#include "WindowLib/NormalFont.h"
#include "PreviewWindow/PreviewWindow.h"

#include "../../resource.h"

#ifndef GETTER
#define GETTER(obj) decltype(obj) &get##obj() { return obj; }
#endif

class UI
{
private:
    std::unique_ptr<MainWindow> _wnd;
    std::unique_ptr<NormalFont> _normal_font;

    std::unique_ptr<PreviewWindow> _preview_wnd;

    std::unique_ptr<Label> _source_wnd_label;
    std::unique_ptr<Label> _resolution_label;
    std::unique_ptr<Label> _x_symbol_label;
    std::unique_ptr<Label> _fps_label;

    std::unique_ptr<Edit> _source_wnd_edit;
    std::unique_ptr<Edit> _width_edit;
    std::unique_ptr<Edit> _height_edit;
    std::unique_ptr<Edit> _fps_edit;

    std::unique_ptr<CheckBox> _capture_optimization_checkbox;
    std::unique_ptr<CheckBox> _capture_cursor_checkbox;

    std::unique_ptr<RadioButton> _capture_client_rect_only_radio_btn;
    std::unique_ptr<RadioButton> _capture_entire_window_radio_btn;

    std::unique_ptr<Button> _settings_apply_button;

    std::unique_ptr<MenuPoint> _preview_chekced_menu_point;
    std::unique_ptr<MenuPoint> _start_recording_menu_point;
    std::unique_ptr<MenuPoint> _stop_recording_menu_point;

    std::unique_ptr<PopupMenu> _options_menu;
    std::unique_ptr<Menu> _wnd_menu;

public:
    UI(const char *app_name, const char *app_version);
    UI(const UI &) = delete;
    ~UI() = default;

    GETTER(_wnd)

    GETTER(_preview_wnd)

    GETTER(_source_wnd_label)
    GETTER(_resolution_label)
    GETTER(_x_symbol_label)
    GETTER(_fps_label)

    GETTER(_source_wnd_edit)
    GETTER(_width_edit)
    GETTER(_height_edit)
    GETTER(_fps_edit)

    GETTER(_capture_optimization_checkbox)
    GETTER(_capture_cursor_checkbox)

    GETTER(_capture_client_rect_only_radio_btn)
    GETTER(_capture_entire_window_radio_btn)

    GETTER(_settings_apply_button)

    GETTER(_preview_chekced_menu_point)
    GETTER(_start_recording_menu_point)
    GETTER(_stop_recording_menu_point)

    GETTER(_options_menu)
    GETTER(_wnd_menu)

};

#endif /* UI_H_ */
