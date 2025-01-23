#ifndef UI_H_
#define UI_H_

#include "../../SmtObj.h"
#include "WindowLib/Window.h"
#include "WindowLib/NormalFont.h"
#include "PreviewWindow/PreviewWindow.h"
#include "DShowWindow/DShowWindow.h"

#include "../../resource.h"

#ifndef GETTER
#define GETTER(obj) decltype(obj) &get##obj() { return obj; }
#endif

class UI
{
private:
    const float _preview_width_coeff = 0.6648f;// 500 / 752
    const int _preview_wnd_width = 500;
    const int _start_pos_x = _preview_wnd_width + 20;
    const int _start_pos_y = 10;
    const int _controls_block_width = 222;

    SmtObj<Window> _wnd;
    SmtObj<NormalFont> _normal_font;

    SmtObj<PreviewWindow> _preview_wnd;
    SmtObj<DShowWindow> _dshow_wnd;

    SmtObj<Label> _video_source_label;

    SmtObj<Label> _video_source_wnd_label;
    SmtObj<Label> _video_resolution_label;
    SmtObj<Label> _video_resolution_x_symbol_label;
    SmtObj<Label> _video_fps_label;

    SmtObj<Label> _video_codec_label;
    SmtObj<Label> _video_quality_label;
    SmtObj<Label> _video_quality_percent_label;

    SmtObj<Label> _audio_source_label;
    SmtObj<Label> _audio_codec_label;

    SmtObj<Edit> _video_source_wnd_edit;
    SmtObj<Edit> _video_width_edit;
    SmtObj<Edit> _video_height_edit;
    SmtObj<Edit> _video_fps_edit;
    SmtObj<Edit> _video_quality_edit;

    SmtObj<ComboBox> _video_sources_list;
    SmtObj<ComboBox> _video_codecs_list;
    SmtObj<ComboBox> _audio_sources_list;
    SmtObj<ComboBox> _audio_codecs_list;

    SmtObj<MenuPoint> _preview_chekced_menu_point;
    SmtObj<MenuPoint> _video_source_menu_point;
    SmtObj<MenuPoint> _video_capture_pin_menu_point;
    SmtObj<MenuPoint> _audio_source_menu_point;
    SmtObj<MenuPoint> _audio_capture_pin_menu_point;
    SmtObj<MenuPoint> _start_recording_menu_point;
    SmtObj<MenuPoint> _stop_recording_menu_point;

    SmtObj<PopupMenu> _options_menu;
    SmtObj<Menu> _wnd_menu;

public:
    UI(const char *app_name, const char *app_version);
    UI(const UI &) = delete;
    ~UI() = default;

    void ResetVideoRecordingRelatedControls();

    GETTER(_wnd)

    GETTER(_preview_wnd)
    GETTER(_dshow_wnd)

    GETTER(_video_source_label)

    GETTER(_video_source_wnd_label)
    GETTER(_video_resolution_label)
    GETTER(_video_resolution_x_symbol_label)
    GETTER(_video_fps_label)

    GETTER(_video_codec_label)
    GETTER(_video_quality_label)
    GETTER(_video_quality_percent_label)

    GETTER(_audio_source_label)
    GETTER(_audio_codec_label)

    GETTER(_video_source_wnd_edit)
    GETTER(_video_width_edit)
    GETTER(_video_height_edit)
    GETTER(_video_fps_edit)
    GETTER(_video_quality_edit)

    GETTER(_video_sources_list)
    GETTER(_video_codecs_list)
    GETTER(_audio_sources_list)
    GETTER(_audio_codecs_list)

    GETTER(_preview_chekced_menu_point)
    GETTER(_video_source_menu_point)
    GETTER(_video_capture_pin_menu_point)
    GETTER(_audio_source_menu_point)
    GETTER(_audio_capture_pin_menu_point)
    GETTER(_start_recording_menu_point)
    GETTER(_stop_recording_menu_point)

    GETTER(_options_menu)
    GETTER(_wnd_menu)

};

#endif /* UI_H_ */
