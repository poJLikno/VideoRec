#include "UI.h"

#define NEW_PREVIEW_WIDTH (500)
#define NEW_PREVIEW_HEIGHT ((int)round((float)NEW_PREVIEW_WIDTH * 9.0f / 16.0f))
#define CTRL_BLOCK_POS_X (10)
#define CTRL_BLOCK_POS_Y (10)

UI::UI(const char *app_name, const char *app_version)
{
    /* Create the main window */
    _wnd = new Window(app_name,
        WndPairValue{ CW_USEDEFAULT, CW_USEDEFAULT },
        WndPairValue{ 520 + 16/* (+16) if-func loose compensation */ /* 520 - client rect */,
        370 + 59/* (+59) if-func loose compensation */ /* 370 - client rect */ },
        IDI_ICON1);
    /* Setup the main window */
    _wnd->SetWndText(std::string(app_name + std::string(" ") + app_version).c_str());
    _wnd->EnableControlsDialogMessages(true);

    /* Create a preview window */
    _preview_wnd = new PreviewWindow(_wnd, WndPairValue{ 10, 10 + 57/* ctrl block height */ + 10}, WndPairValue{500, 281});
    _preview_wnd->ShowWnd(false);
    
    /* Create window controls */
    /* Labels */
    _video_source_wnd_label = new Label("Captured window: ",
        WndPairValue{ CTRL_BLOCK_POS_X, CTRL_BLOCK_POS_Y }, WndPairValue{ 100, 17 });

    _video_resolution_label = new Label("Video resolution: ",
        WndPairValue{ CTRL_BLOCK_POS_X, CTRL_BLOCK_POS_Y + 20 }, WndPairValue{ 100, 17 });

    _video_resolution_x_symbol_label = new Label("X",
        WndPairValue{ CTRL_BLOCK_POS_X + 164, CTRL_BLOCK_POS_Y + 20 }, WndPairValue{ 10, 17 });

    _video_fps_label = new Label("Video FPS: ",
        WndPairValue{ CTRL_BLOCK_POS_X, CTRL_BLOCK_POS_Y + 40 }, WndPairValue{ 60, 17 });

    /* Edits */
    _video_source_wnd_edit = new Edit(
        WndPairValue{ CTRL_BLOCK_POS_X + 110, CTRL_BLOCK_POS_Y }, WndPairValue{ 116, 17 });

    _video_width_edit = new Edit(
        WndPairValue{ CTRL_BLOCK_POS_X + 110, CTRL_BLOCK_POS_Y + 20 }, WndPairValue{ 48, 17 });

    _video_height_edit = new Edit(
        WndPairValue{ CTRL_BLOCK_POS_X + 178, CTRL_BLOCK_POS_Y + 20 }, WndPairValue{ 48, 17 });

    _video_fps_edit = new Edit(
        WndPairValue{ CTRL_BLOCK_POS_X + 110, CTRL_BLOCK_POS_Y + 40 }, WndPairValue{ 30, 17 });

    /* Checkboxes */
    _video_capture_optimization_checkbox = new CheckBox("Optimization",
        WndPairValue{ CTRL_BLOCK_POS_X + 150, CTRL_BLOCK_POS_Y + 40 }, WndPairValue{ 90, 17 });

    _video_capture_cursor_checkbox = new CheckBox("Capture cursor",
        WndPairValue{ CTRL_BLOCK_POS_X + 250, CTRL_BLOCK_POS_Y + 40 }, WndPairValue{ 100, 17 });

    /* Radio Buttons */
    _video_capture_client_rect_only_radio_btn = new RadioButton("Capture client region only", false,
        WndPairValue{ CTRL_BLOCK_POS_X + 246, CTRL_BLOCK_POS_Y }, WndPairValue{ 160, 17 });

    _video_capture_entire_screen_radio_btn = new RadioButton("Capture entire window", true,
        WndPairValue{ CTRL_BLOCK_POS_X + 246, CTRL_BLOCK_POS_Y + 20 }, WndPairValue{ 160, 17 });

    /* Buttons */
    _video_settings_apply_button = new Button("Apply", 
        WndPairValue{ CTRL_BLOCK_POS_X + 426, CTRL_BLOCK_POS_Y }, WndPairValue{ 70, 37 });

    /* Menus */
    _preview_chekced_menu_point = new MenuPoint("Preview", Checked);
    _start_recording_menu_point = new MenuPoint("Start recording (key: Alt + K)");
    _stop_recording_menu_point = new MenuPoint("Stop recording (key: Alt + K)");

    _options_menu = new PopupMenu("Options");
    _wnd_menu = new Menu();

    /* Setup the menu points relations */
    _options_menu->AttachMenuPoint(_preview_chekced_menu_point);
    _options_menu->AppendSeparator();
    _options_menu->AttachMenuPoint(_start_recording_menu_point);
    _options_menu->AttachMenuPoint(_stop_recording_menu_point);

    _wnd_menu->AttachPopupMenu(_options_menu);

    /* Attach controls to window */
    _wnd->AttachChildControl(_video_source_wnd_label);
    _wnd->AttachChildControl(_video_resolution_label);
    _wnd->AttachChildControl(_video_resolution_x_symbol_label);
    _wnd->AttachChildControl(_video_fps_label);

    _wnd->AttachChildControl(_video_source_wnd_edit);
    _wnd->AttachChildControl(_video_width_edit);
    _wnd->AttachChildControl(_video_height_edit);
    _wnd->AttachChildControl(_video_fps_edit);

    _wnd->AttachChildControl(_video_capture_optimization_checkbox);
    _wnd->AttachChildControl(_video_capture_cursor_checkbox);

    _wnd->AttachChildControl(_video_capture_client_rect_only_radio_btn);
    _wnd->AttachChildControl(_video_capture_entire_screen_radio_btn);

    _wnd->AttachChildControl(_video_settings_apply_button);

    _wnd->AttachMenu(_wnd_menu);

    /* Normal font */
    _normal_font = new NormalFont();
    _normal_font->SetFont(_video_source_wnd_label);
    _normal_font->SetFont(_video_resolution_label);
    _normal_font->SetFont(_video_resolution_x_symbol_label);
    _normal_font->SetFont(_video_fps_label);

    _normal_font->SetFont(_video_source_wnd_edit);
    _normal_font->SetFont(_video_width_edit);
    _normal_font->SetFont(_video_height_edit);
    _normal_font->SetFont(_video_fps_edit);

    _normal_font->SetFont(_video_capture_optimization_checkbox);
    _normal_font->SetFont(_video_capture_cursor_checkbox);

    _normal_font->SetFont(_video_capture_client_rect_only_radio_btn);
    _normal_font->SetFont(_video_capture_entire_screen_radio_btn);

    _normal_font->SetFont(_video_settings_apply_button);

    /* Transform callbacks */
#undef NEW_PREVIEW_WIDTH
#define NEW_PREVIEW_WIDTH (new_parent_size.first - 20)

    /* The main window sides relstion callback (will be attached to last label parent resize callback) */
    auto main_wnd_sides_callback = [this](void *ptr)->void {
        GetMiscForParentResize(ptr);

        if (_preview_chekced_menu_point->GetState())
        {
            int preview_wnd_bottom_pos = _preview_wnd->GetWndPos().second + _preview_wnd->GetWndSize().second;
            if (parent_wnd->GetWndSize().second - preview_wnd_bottom_pos != 12)
            {
                parent_wnd->SetWndSize(WndPairValue{ new_parent_size.first /*!!!*/ + 16/* (+16) for in-function loose compensation */,
                    preview_wnd_bottom_pos + 12 /*!!!*/ + 59/* (+59) for in-function loose compensation */ });
            }
        }
        else
        {
            if (parent_wnd->GetWndSize().second != 77)
            {
                parent_wnd->SetWndSize(WndPairValue{ new_parent_size.first /*!!!*/ + 16/* (+16) for in-function loose compensation */,
                    77 /*!!!*/ + 59/* (+59) for in-function loose compensation */ });
            }
        }
        
        };

    auto preview_wnd_resize_callback = [this](void *ptr)->void {
        GetMiscForParentResize(ptr);

        _preview_wnd->SetWndSize(WndPairValue{ NEW_PREVIEW_WIDTH, NEW_PREVIEW_HEIGHT });
        };

    /* Labels */
    _video_source_wnd_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        label->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X, CTRL_BLOCK_POS_Y });
        });

    _video_resolution_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        label->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X, CTRL_BLOCK_POS_Y + 20 });
        });

    _video_resolution_x_symbol_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        label->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 110 + /* Video width edit size */(NEW_PREVIEW_WIDTH * 48 / 500) + 5, CTRL_BLOCK_POS_Y + 20});
        });

    _video_fps_label->AddCallback("ParentResizeCallback", [this, preview_wnd_resize_callback, main_wnd_sides_callback](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        label->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X, CTRL_BLOCK_POS_Y + 40 });

         /* Preview window resize */
        preview_wnd_resize_callback(ptr);

        /* Window's sides proportion */
        main_wnd_sides_callback(ptr);
        });


    /* Edits */
    _video_source_wnd_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Edit *edit = GetControlForParentResize(Edit, ptr);
        GetMiscForParentResize(ptr);

        edit->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 110, CTRL_BLOCK_POS_Y });
        edit->SetWndSize(WndPairValue{ NEW_PREVIEW_WIDTH * 96 / 500 + 20, 17 });
        });

    _video_width_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Edit *edit = GetControlForParentResize(Edit, ptr);
        GetMiscForParentResize(ptr);

        edit->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 110, CTRL_BLOCK_POS_Y + 20 });
        edit->SetWndSize(WndPairValue{ NEW_PREVIEW_WIDTH * 48 / 500, 17 });
        });

    _video_height_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Edit *edit = GetControlForParentResize(Edit, ptr);
        GetMiscForParentResize(ptr);

        edit->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 110 + /* Video width edit size */(NEW_PREVIEW_WIDTH * 48 / 500) + 20, CTRL_BLOCK_POS_Y + 20 });
        edit->SetWndSize(WndPairValue{ NEW_PREVIEW_WIDTH * 48 / 500, 17 });
        });

    _video_fps_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Edit *edit = GetControlForParentResize(Edit, ptr);
        GetMiscForParentResize(ptr);

        edit->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 110, CTRL_BLOCK_POS_Y + 40 });
        });

    /* Checkboxes */
    _video_capture_optimization_checkbox->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        CheckBox *button = GetControlForParentResize(CheckBox, ptr);
        GetMiscForParentResize(ptr);

        button->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 150, CTRL_BLOCK_POS_Y + 40 });
        });

    _video_capture_cursor_checkbox->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        CheckBox *button = GetControlForParentResize(CheckBox, ptr);
        GetMiscForParentResize(ptr);

        button->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 250, CTRL_BLOCK_POS_Y + 40 });
        });

    /* Radio Buttons */
    _video_capture_client_rect_only_radio_btn->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Button *button = GetControlForParentResize(Button, ptr);
        GetMiscForParentResize(ptr);

        button->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 110 + /* Captured wnd name edit size */(NEW_PREVIEW_WIDTH * 96 / 500 + 20) + 20, CTRL_BLOCK_POS_Y});
        });

    _video_capture_entire_screen_radio_btn->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Button *button = GetControlForParentResize(Button, ptr);
        GetMiscForParentResize(ptr);

        button->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 110 + /* Captured wnd name edit size */(NEW_PREVIEW_WIDTH * 96 / 500 + 20) + 20, CTRL_BLOCK_POS_Y + 20 });
        });

    /* Buttons */
    _video_settings_apply_button->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Button *button = GetControlForParentResize(Button, ptr);
        GetMiscForParentResize(ptr);

        button->SetWndPos(WndPairValue{ CTRL_BLOCK_POS_X + 110 + /* Captured wnd name edit size */(NEW_PREVIEW_WIDTH * 96 / 500 + 20) + 20 + /* Radio button size */160 + 20, CTRL_BLOCK_POS_Y });
        });
}
