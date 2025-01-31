#include "UI.h"

#define CTRL_BLOCK_POS_X (10)
#define CTRL_BLOCK_POS_Y (20)

UI::UI(const char *app_name, const char *app_version)
{
    /* Create the main window */
    _wnd = new Window(app_name,
        WndPairValue{ CW_USEDEFAULT, CW_USEDEFAULT },
        WndPairValue{ 360 + 16/* 752 - client rect */, 70 + 59/* 301 - client rect */ }, IDI_ICON1,
        WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX));
    /* Setup the main window */
    _wnd->SetWndText(std::string(app_name + std::string(" ") + app_version).c_str());
    _wnd->EnableControlsDialogMessages(true);
    
    /* Create window controls */
    /* Labels */
    _video_resolution_label = new Label("Video resolution: ",
        WndPairValue{ CTRL_BLOCK_POS_X, CTRL_BLOCK_POS_Y }, WndPairValue{ 100, 17 });

    _video_resolution_x_symbol_label = new Label("X",
        WndPairValue{ CTRL_BLOCK_POS_X + 164, CTRL_BLOCK_POS_Y }, WndPairValue{ 10, 17 });

    _video_fps_label = new Label("Video FPS: ",
        WndPairValue{ CTRL_BLOCK_POS_X, CTRL_BLOCK_POS_Y + 20 }, WndPairValue{ 60, 17 });

    /* Edits */
    _video_width_edit = new Edit(
        WndPairValue{ CTRL_BLOCK_POS_X + 110, CTRL_BLOCK_POS_Y }, WndPairValue{ 48, 17 });

    _video_height_edit = new Edit(
        WndPairValue{ CTRL_BLOCK_POS_X + 178, CTRL_BLOCK_POS_Y }, WndPairValue{ 48, 17 });

    _video_fps_edit = new Edit(
        WndPairValue{ CTRL_BLOCK_POS_X + 110, CTRL_BLOCK_POS_Y + 20 }, WndPairValue{ 30, 17 });

    /* Buttons */
    _video_settings_apply_button = new Button("Apply", 
        WndPairValue{ CTRL_BLOCK_POS_X + 246, CTRL_BLOCK_POS_Y }, WndPairValue{ 70, 37 });

    /* Menus */
    _start_recording_menu_point = new MenuPoint("Start recording (key: Alt + K)");
    _stop_recording_menu_point = new MenuPoint("Stop recording (key: Alt + K)");

    _options_menu = new PopupMenu("Options");
    _wnd_menu = new Menu();

    /* Setup the menu points relations */
    _options_menu->AttachMenuPoint(_start_recording_menu_point);
    _options_menu->AttachMenuPoint(_stop_recording_menu_point);

    _wnd_menu->AttachPopupMenu(_options_menu);

    /* Attach controls to window */
    _wnd->AttachChildControl(_video_resolution_label);
    _wnd->AttachChildControl(_video_resolution_x_symbol_label);
    _wnd->AttachChildControl(_video_fps_label);

    _wnd->AttachChildControl(_video_width_edit);
    _wnd->AttachChildControl(_video_height_edit);
    _wnd->AttachChildControl(_video_fps_edit);

    _wnd->AttachChildControl(_video_settings_apply_button);

    _wnd->AttachMenu(_wnd_menu);

    /* Normal font */
    _normal_font = new NormalFont();
    _normal_font->SetFont(_video_resolution_label);
    _normal_font->SetFont(_video_resolution_x_symbol_label);
    _normal_font->SetFont(_video_fps_label);

    _normal_font->SetFont(_video_width_edit);
    _normal_font->SetFont(_video_height_edit);
    _normal_font->SetFont(_video_fps_edit);

    _normal_font->SetFont(_video_settings_apply_button);

    /* Default controls' settings */
    _video_fps_edit->SetWndText("30");
    _video_settings_apply_button->SetInputState(false);

    _stop_recording_menu_point->SetState(false);

    /* No need. Remove main window style WS_THICKFRAME (WS_RESIZEBOX) */

    /* Transform callbacks */
    ///* The main window sides relstion callback (will be attached to last label parent resize callback) */
    //auto main_wnd_sides_callback = [this](void *ptr)->void {
    //    GetMiscForParentResize(ptr);
    //
    //    if ((int)round((float)new_parent_size.first * 1000 / (float)new_parent_size.second) != (int)round(360.0f * 1000 / 70.0f)) {
    //        parent_wnd->SetWndSize(WndPairValue{ new_parent_size.first + 16, (int)round((float)(new_parent_size.first) * 70.0f / 360.0f) + 59 });
    //    }
    //    };
    //
    ///* Labels */
    //_video_resolution_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
    //    Label *label = GetControlForParentResize(Label, ptr);
    //    GetMiscForParentResize(ptr);
    //
    //    label->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X, CTRL_BLOCK_POS_Y });
    //    });
    //
    //_video_resolution_x_symbol_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
    //    Label *label = GetControlForParentResize(Label, ptr);
    //    GetMiscForParentResize(ptr);
    //
    //    label->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 110 + /* Video width edit size */(new_parent_size.first * 48 / 360) + 5, CTRL_BLOCK_POS_Y});
    //    });
    //
    //_video_fps_label->AddCallback("ParentResizeCallback", [this, main_wnd_sides_callback](void *ptr)->void {
    //    Label *label = GetControlForParentResize(Label, ptr);
    //    GetMiscForParentResize(ptr);
    //
    //    label->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X, CTRL_BLOCK_POS_Y + 20 });
    //
    //    /* Window's sides proportion */
    //    main_wnd_sides_callback(ptr);
    //    });
    //
    //
    ///* Edits */
    //_video_width_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
    //    Edit *edit = GetControlForParentResize(Edit, ptr);
    //    GetMiscForParentResize(ptr);
    //
    //    edit->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 110, CTRL_BLOCK_POS_Y });
    //    edit->SetWndSize(WndPairValue{ new_parent_size.first * 48 / 360, 17 });
    //    });
    //
    //_video_height_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
    //    Edit *edit = GetControlForParentResize(Edit, ptr);
    //    GetMiscForParentResize(ptr);
    //
    //    edit->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 110 + /* Video width edit size */(new_parent_size.first * 48 / 360) + 20, CTRL_BLOCK_POS_Y });
    //    edit->SetWndSize(WndPairValue{ new_parent_size.first * 48 / 360, 17 });
    //    });
    //
    //_video_fps_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
    //    Edit *edit = GetControlForParentResize(Edit, ptr);
    //    GetMiscForParentResize(ptr);
    //
    //    edit->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 110, CTRL_BLOCK_POS_Y + 20 });
    //    });
    //
    ///* Buttons */
    //_video_settings_apply_button->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
    //    Button *button = GetControlForParentResize(Button, ptr);
    //    GetMiscForParentResize(ptr);
    //
    //    button->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 110 + /* Captured wnd name edit size */(new_parent_size.first * 96 / 360 + 20) + 20, CTRL_BLOCK_POS_Y });
    //    });
}
