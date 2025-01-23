#include "UI.h"

UI::UI(const char *app_name, const char *app_version)
{
    /* Create the main window */
    _wnd = new Window(app_name,
        WndPairValue{ CW_USEDEFAULT, CW_USEDEFAULT },
        WndPairValue{ 768/*752*/, 301 + 59/*482/*423*/ }, IDI_ICON1);
    /* Setup the main window */
    _wnd->SetWndText(std::string(app_name + std::string(" ") + app_version).c_str());
    _wnd->EnableControlsDialogMessages(true);

    /* Create a preview window */
    _preview_wnd = new PreviewWindow(_wnd, WndPairValue{ 10, 10 }, WndPairValue{ 500, 281 });
    _dshow_wnd = new DShowWindow();
    _dshow_wnd->PutOwner(_wnd->GetHwnd());
    _dshow_wnd->PutStyle(WS_VISIBLE | WS_CHILD);
    _dshow_wnd->PutSize(500u, 281u);
    _dshow_wnd->PutPos(10u, 10u);
    
    /* Create window controls */
    /* Labels */
    _video_source_label = new Label("Video source: ",
        WndPairValue{ _start_pos_x, _start_pos_y }, WndPairValue{ 130, 17 });

    _video_source_wnd_label = new Label("Captured window: ",
        WndPairValue{ _start_pos_x, _start_pos_y + 50 }, WndPairValue{ 100, 17 });

    _video_resolution_label = new Label("Video resolution: ",
        WndPairValue{ _start_pos_x, _start_pos_y + 70 }, WndPairValue{ 100, 17 });

    _video_resolution_x_symbol_label = new Label("X",
        WndPairValue{ _start_pos_x + 145, _start_pos_y + 70 }, WndPairValue{ 10, 17 });

    _video_fps_label = new Label("Video FPS: ",
        WndPairValue{ _start_pos_x, _start_pos_y + 90 }, WndPairValue{ 60, 17 });

    _video_codec_label = new Label("Video codec: ",
        WndPairValue{ _start_pos_x, _start_pos_y + 50 }, WndPairValue{ 80, 17 });

    _video_quality_label = new Label("Quality: ",
        WndPairValue{ _start_pos_x + 10, _start_pos_y + 70 }, WndPairValue{ 42, 17 });

    _video_quality_percent_label = new Label("%",
        WndPairValue{ _start_pos_x + 95, _start_pos_y + 70 }, WndPairValue{ 12, 17 });

    _audio_source_label = new Label("Audio source: ",
        WndPairValue{ _start_pos_x, _start_pos_y + 135 }, WndPairValue{ 130, 17 });

    _audio_codec_label = new Label("Audio codec: ",
        WndPairValue{ _start_pos_x, _start_pos_y + 185 }, WndPairValue{ 80, 17 });

    /* Edits */
    _video_source_wnd_edit = new Edit(
        WndPairValue{ _start_pos_x + 110, _start_pos_y + 50 }, WndPairValue{ 50, 17 });

    _video_width_edit = new Edit(
        WndPairValue{ _start_pos_x + 110, _start_pos_y + 70 }, WndPairValue{ 30, 17 });

    _video_height_edit = new Edit(
        WndPairValue{ _start_pos_x + 160, _start_pos_y + 70 }, WndPairValue{ 30, 17 });

    _video_fps_edit = new Edit(
        WndPairValue{ _start_pos_x + 110, _start_pos_y + 90 }, WndPairValue{ 30, 17 });

    _video_quality_edit = new Edit(
        WndPairValue{ _start_pos_x + 60, _start_pos_y + 70 }, WndPairValue{ 30, 17 });

    /* Combo boxes */
    _video_sources_list = new ComboBox(
        WndPairValue{ _start_pos_x, _start_pos_y + 20 }, WndPairValue{ _controls_block_width, 50 });

    _video_codecs_list = new ComboBox(
        WndPairValue{ _start_pos_x + 90, _start_pos_y + 45 }, WndPairValue{ _controls_block_width - 90, 50 });

    _audio_sources_list = new ComboBox(
        WndPairValue{ _start_pos_x, _start_pos_y + 155 }, WndPairValue{ _controls_block_width, 50 });

    _audio_codecs_list = new ComboBox(
        WndPairValue{ _start_pos_x + 90, _start_pos_y + 180 }, WndPairValue{ _controls_block_width - 90, 50 });

    /* Menus */
    _preview_chekced_menu_point = new MenuPoint("Preview", Checked);
    _video_source_menu_point = new MenuPoint("Video source settings");
    _video_capture_pin_menu_point = new MenuPoint("Video capture pin settings");
    _audio_source_menu_point = new MenuPoint("Audio source settings");
    _audio_capture_pin_menu_point = new MenuPoint("Audio capture pin settings");
    _start_recording_menu_point = new MenuPoint("Start recording (key: Alt + K)");
    _stop_recording_menu_point = new MenuPoint("Stop recording (key: Alt + K)");

    _options_menu = new PopupMenu("Options");
    _wnd_menu = new Menu();

    /* Setup the menu points relations */
    _options_menu->AttachMenuPoint(_preview_chekced_menu_point);
    _options_menu->AppendSeparator();
    _options_menu->AttachMenuPoint(_video_source_menu_point);
    _options_menu->AttachMenuPoint(_video_capture_pin_menu_point);
    _options_menu->AppendSeparator();
    _options_menu->AttachMenuPoint(_audio_source_menu_point);
    _options_menu->AttachMenuPoint(_audio_capture_pin_menu_point);
    _options_menu->AppendSeparator();
    _options_menu->AttachMenuPoint(_start_recording_menu_point);
    _options_menu->AttachMenuPoint(_stop_recording_menu_point);

    _wnd_menu->AttachPopupMenu(_options_menu);

    /* Attach controls to window */
    _wnd->AttachChildControl(_video_source_label);
    _wnd->AttachChildControl(_video_source_wnd_label);
    _wnd->AttachChildControl(_video_resolution_label);
    _wnd->AttachChildControl(_video_resolution_x_symbol_label);
    _wnd->AttachChildControl(_video_fps_label);
    _wnd->AttachChildControl(_video_codec_label);
    _wnd->AttachChildControl(_video_quality_label);
    _wnd->AttachChildControl(_video_quality_percent_label);
    _wnd->AttachChildControl(_audio_source_label);
    _wnd->AttachChildControl(_audio_codec_label);

    _wnd->AttachChildControl(_video_source_wnd_edit);
    _wnd->AttachChildControl(_video_width_edit);
    _wnd->AttachChildControl(_video_height_edit);
    _wnd->AttachChildControl(_video_fps_edit);
    _wnd->AttachChildControl(_video_quality_edit);

    _wnd->AttachChildControl(_video_sources_list);
    _wnd->AttachChildControl(_video_codecs_list);
    _wnd->AttachChildControl(_audio_sources_list);
    _wnd->AttachChildControl(_audio_codecs_list);

    _wnd->AttachMenu(_wnd_menu);

    /* Normal font */
    _normal_font = new NormalFont();
    _normal_font->SetFont(_video_source_label);
    _normal_font->SetFont(_video_source_wnd_label);
    _normal_font->SetFont(_video_resolution_label);
    _normal_font->SetFont(_video_resolution_x_symbol_label);
    _normal_font->SetFont(_video_fps_label);
    _normal_font->SetFont(_video_codec_label);
    _normal_font->SetFont(_video_quality_label);
    _normal_font->SetFont(_video_quality_percent_label);
    _normal_font->SetFont(_audio_source_label);
    _normal_font->SetFont(_audio_codec_label);

    _normal_font->SetFont(_video_source_wnd_edit);
    _normal_font->SetFont(_video_width_edit);
    _normal_font->SetFont(_video_height_edit);
    _normal_font->SetFont(_video_fps_edit);
    _normal_font->SetFont(_video_quality_edit);

    _normal_font->SetFont(_video_sources_list);
    _normal_font->SetFont(_video_codecs_list);
    _normal_font->SetFont(_audio_sources_list);
    _normal_font->SetFont(_audio_codecs_list);

    /* Default controls' settings */
    _video_fps_edit->SetWndText("30");
    _video_quality_edit->SetWndText("0");

    _video_sources_list->AddItem("None");
    _video_codecs_list->AddItem("None");
    _audio_sources_list->AddItem("None");
    _audio_codecs_list->AddItem("None");
    _video_sources_list->SelectItem(0);
    _video_codecs_list->SelectItem(0);
    _audio_sources_list->SelectItem(0);
    _audio_codecs_list->SelectItem(0);

    _preview_wnd->ShowWnd(false);


    _video_source_wnd_label->ShowWnd(false);
    _video_source_wnd_edit->ShowWnd(false);
    _video_resolution_label->ShowWnd(false);
    _video_resolution_x_symbol_label->ShowWnd(false);
    _video_width_edit->ShowWnd(false);
    _video_height_edit->ShowWnd(false);
    _video_fps_label->ShowWnd(false);
    _video_fps_edit->ShowWnd(false);

    _video_codec_label->ShowWnd(false);
    _video_codecs_list->ShowWnd(false);

    _video_quality_label->ShowWnd(false);
    _video_quality_percent_label->ShowWnd(false);
    _video_quality_edit->ShowWnd(false);

    _video_source_menu_point->SetState(false);
    _video_capture_pin_menu_point->SetState(false);
    _audio_source_menu_point->SetState(false);
    _audio_capture_pin_menu_point->SetState(false);
    _stop_recording_menu_point->SetState(false);

    /* Transform callbacks */
    /* The main window sides relstion callback (will be attached to last label parent resize callback) */
    auto main_wnd_sides_callback = [this](void *ptr)->void {
        GetMiscForParentResize(ptr);

        if (round(2.498f * (float)new_parent_size.second) != (float)new_parent_size.first) {
            parent_wnd->SetWndSize(WndPairValue{ new_parent_size.first + 16, (int)round(1.f / 2.498f * (float)new_parent_size.first) + 59 });
        }
        };

    auto preview_wnd_resize_callback = [this](void *ptr)->void {
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);

        _preview_wnd->SetWndSize(WndPairValue{ (long)new_preview_width, (long)round(9.f / 16.f * (float)new_preview_width) });
        };

    auto dshow_wnd_resize_callback = [this](void *ptr)->void {
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);

        _dshow_wnd->PutSize((long)new_preview_width, (long)round(9.f / 16.f * (float)new_preview_width));
        };

    /* Labels */
    _video_source_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        /* Set video source label new position */
        label->SetWndPos(WndPairValue{ new_preview_width + 20, _start_pos_y });
        });

    _video_source_wnd_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        label->SetWndPos(WndPairValue{ new_preview_width + 20, _start_pos_y + 50 });
        });

    _video_resolution_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        label->SetWndPos(WndPairValue{ new_preview_width + 20, _start_pos_y + 70 });
        });

    _video_resolution_x_symbol_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        label->SetWndPos(WndPairValue{ new_preview_width + 20 + 100 + 10 + 5 + (new_parent_size.first - new_preview_width - 100 - 10 - 30 - 20) / 2, _start_pos_y + 70 });
        });

    _video_fps_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        label->SetWndPos(WndPairValue{ new_preview_width + 20, _start_pos_y + 90 });
        });

    _video_codec_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        label->SetWndPos(WndPairValue{ new_preview_width + 20, _start_pos_y + 50 });
        });

    _video_quality_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        label->SetWndPos(WndPairValue{ new_preview_width + 30, _start_pos_y + 70 });
        });

    _video_quality_percent_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        label->SetWndPos(WndPairValue{ new_preview_width + 115, _start_pos_y + 70 });
        });

    _audio_source_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        label->SetWndPos(WndPairValue{ new_preview_width + 20, _start_pos_y + 135 });
        });

    _audio_codec_label->AddCallback("ParentResizeCallback", [this, main_wnd_sides_callback, preview_wnd_resize_callback, dshow_wnd_resize_callback](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        label->SetWndPos(WndPairValue{ new_preview_width + 20, _start_pos_y + 185 });

        /* DShow preview window resize */
        dshow_wnd_resize_callback(ptr);

        /* Preview window resize */
        preview_wnd_resize_callback(ptr);

        /* Window's sides proportion */
        main_wnd_sides_callback(ptr);
        });

    /* Edits */
    _video_source_wnd_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Edit *edit = GetControlForParentResize(Edit, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        edit->SetWndPos(WndPairValue{ new_preview_width + 130, _start_pos_y + 50 });
        edit->SetWndSize(WndPairValue{ new_parent_size.first - new_preview_width - 100 - 10 - 30, 17 });
        });

    _video_width_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Edit *edit = GetControlForParentResize(Edit, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        edit->SetWndPos(WndPairValue{ new_preview_width + 130, _start_pos_y + 70 });
        edit->SetWndSize(WndPairValue{ (new_parent_size.first - new_preview_width - 100 - 10 - 30 - 20) / 2, 17 });
        });

    _video_height_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Edit *edit = GetControlForParentResize(Edit, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        edit->SetWndPos(WndPairValue{ new_preview_width + 130 + 20 + (new_parent_size.first - new_preview_width - 100 - 10 - 30 - 20) / 2, _start_pos_y + 70 });
        edit->SetWndSize(WndPairValue{ (new_parent_size.first - new_preview_width - 100 - 10 - 30 - 20) / 2, 17 });
        });

    _video_fps_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Edit *edit = GetControlForParentResize(Edit, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        edit->SetWndPos(WndPairValue{ new_preview_width + 130, _start_pos_y + 90 });
        });

    _video_quality_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Edit *edit = GetControlForParentResize(Edit, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        edit->SetWndPos(WndPairValue{ new_preview_width + 80, _start_pos_y + 70 });
        });

    /* Combo boxes */
    _video_sources_list->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        ComboBox *list = GetControlForParentResize(ComboBox, ptr);
        GetMiscForParentResize(ptr);

        int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        list->SetWndPos(WndPairValue{ new_preview_width + 20, _start_pos_y + 20 });
        list->SetWndSize(WndPairValue{ new_parent_size.first - new_preview_width - 30, 50 });
        });

    _video_codecs_list->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        ComboBox *list = GetControlForParentResize(ComboBox, ptr);
        GetMiscForParentResize(ptr);

            int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        list->SetWndPos(WndPairValue{ new_preview_width + 110, _start_pos_y + 45 });
        list->SetWndSize(WndPairValue{ new_parent_size.first - new_preview_width - 120, 50 });
        });

    _audio_sources_list->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        ComboBox *list = GetControlForParentResize(ComboBox, ptr);
        GetMiscForParentResize(ptr);

            int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        list->SetWndPos(WndPairValue{ new_preview_width + 20, _start_pos_y + 155 });
        list->SetWndSize(WndPairValue{ new_parent_size.first - new_preview_width - 30, 50 });
        });

    _audio_codecs_list->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        ComboBox *list = GetControlForParentResize(ComboBox, ptr);
        GetMiscForParentResize(ptr);

            int new_preview_width = (int)round((float)new_parent_size.first * _preview_width_coeff);
        list->SetWndPos(WndPairValue{ new_preview_width + 110, _start_pos_y + 180 });
        list->SetWndSize(WndPairValue{ new_parent_size.first - new_preview_width - 120, 50 });
        });
}

void UI::ResetVideoRecordingRelatedControls()
{
    _preview_wnd->SetSrc(nullptr, 0, 0);
    _preview_wnd->ShowWnd(false);

    _video_source_wnd_label->ShowWnd(false);
    _video_source_wnd_edit->ShowWnd(false);
    _video_resolution_label->ShowWnd(false);
    _video_resolution_x_symbol_label->ShowWnd(false);
    _video_width_edit->ShowWnd(false);
    _video_height_edit->ShowWnd(false);
    _video_fps_label->ShowWnd(false);
    _video_fps_edit->ShowWnd(false);

    _video_codec_label->ShowWnd(false);
    _video_codecs_list->ShowWnd(false);

    _video_quality_label->ShowWnd(false);
    _video_quality_percent_label->ShowWnd(false);
    _video_quality_edit->ShowWnd(false);

    _video_source_menu_point->SetState(false);
    _video_capture_pin_menu_point->SetState(false);

    _start_recording_menu_point->SetState(true);
    _stop_recording_menu_point->SetState(false);
}
