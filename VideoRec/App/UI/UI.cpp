#include "UI.h"

#define CtrlBlockX (10)
#define CtrlBlockY (10)
//#define CtrlBlockWidth (?)
#define CtrlBlockHeight (SourceWndLabelHeight + 3/*gap*/ + ResolutionLabelHeight + 3/*gap*/ + FpsLabelHeight)

/* Main window */
#define MainWndWidth (520 + 16 /* 520 - client rect */)
#define MainWndHeight (370 + 59 /* 370 - client rect */)
/**/
#define NewMainWndHeightFull (preview_wnd_client_bottom_pos + 12/*gap*/ + parent_wnd->GetWndHeaderSize() + parent_wnd->GetWndBottomBorderSize())
#define NewMainWndHeightShort (77/*client height*/ + parent_wnd->GetWndHeaderSize() + parent_wnd->GetWndBottomBorderSize())

/* Preview window */
#define PreviewWndX (CtrlBlockX)
#define PreviewWndY (CtrlBlockY + CtrlBlockHeight + 10/*gap*/)
#define PreviewWndWidth (500)
#define PreviewWndHeight (281)
/**/
#define NewPreviewWndWidth (parent_wnd->GetWndClientSize().first - (2 * 10)/*gap*/)
#define NewPreviewWndHeight ((int)round((float)NewPreviewWndWidth * 9.0f / 16.0f))

/* Labels */
#define SourceWndLabelX (CtrlBlockX)
#define SourceWndLabelY (CtrlBlockY)
#define SourceWndLabelWidth (100)
#define SourceWndLabelHeight (17)

#define ResolutionLabelX (SourceWndLabelX)
#define ResolutionLabelY (SourceWndLabelY + SourceWndLabelHeight + 3/*gap*/)
#define ResolutionLabelWidth (100)
#define ResolutionLabelHeight (17)

#define XSymbolLabelX (WidthEditX + WidthEditWidth + 5/*gap*/)
#define XSymbolLabelY (ResolutionLabelY)
#define XSymbolLabelWidth (10)
#define XSymbolLabelHeight (17)
/**/
#define NewXSymbolLabelX (WidthEditX + NewWidthEditWidth + 5/*gap*/)

#define FpsLabelX (CtrlBlockX)
#define FpsLabelY (ResolutionLabelY + ResolutionLabelHeight + 3/*gap*/)
#define FpsLabelWidth (60)
#define FpsLabelHeight (17)

/* Edits */
#define SourceWndEditX (SourceWndLabelX + SourceWndLabelWidth + 10/*gap*/)
#define SourceWndEditY (SourceWndLabelY)
#define SourceWndEditWidth (WidthEditWidth + 5/*gap*/ + XSymbolLabelWidth + 3/*gap*/ + HeightEditWidth)
#define SourceWndEditHeight (17)
/**/
#define NewSourceWndEditWidth (NewWidthEditWidth + 5/*gap*/ + XSymbolLabelWidth + 3/*gap*/ + NewHeightEditWidth)

#define WidthEditX (ResolutionLabelX + ResolutionLabelWidth + 10/*gap*/)
#define WidthEditY (ResolutionLabelY)
#define WidthEditWidth (48)
#define WidthEditHeight (17)
/**/
#define NewWidthEditWidth (NewPreviewWndWidth * WidthEditWidth / PreviewWndWidth)

#define HeightEditX (XSymbolLabelX + XSymbolLabelWidth + 3/*gap*/)
#define HeightEditY (ResolutionLabelY)
#define HeightEditWidth (48)
#define HeightEditHeight (17)
/**/
#define NewHeightEditX (NewXSymbolLabelX + XSymbolLabelWidth + 3/*gap*/)
#define NewHeightEditWidth (NewPreviewWndWidth * HeightEditWidth / PreviewWndWidth)

#define FpsEditX (WidthEditX)
#define FpsEditY (FpsLabelY)
#define FpsEditWidth (30)
#define FpsEditHeight (17)

/* Checkboxes */
#define CaptureCursorCheckboxX (FpsEditX + FpsEditWidth + 10/*gap*/)
#define CaptureCursorCheckboxY (FpsLabelY)
#define CaptureCursorCheckboxWidth (100)
#define CaptureCursorCheckboxHeight (17)

#define CaptureOptimizationCheckboxX (CaptureEntireWindowRadioBtnX + 16/*gap*/)
#define CaptureOptimizationCheckboxY (CaptureEntireWindowRadioBtnY + CaptureEntireWindowRadioBtnHeight + 3/*gap*/)
#define CaptureOptimizationCheckboxWidth (90)
#define CaptureOptimizationCheckboxHeight (17)
/**/
#define NewCaptureOptimizationCheckboxX (NewCaptureEntireWindowRadioBtnX + 16/*gap*/)

/* Radio buttons */
#define CaptureClientRectOnlyRadioBtnX (SourceWndEditX + SourceWndEditWidth + 22/*gap*/)
#define CaptureClientRectOnlyRadioBtnY (SourceWndLabelY)
#define CaptureClientRectOnlyRadioBtnWidth (160)
#define CaptureClientRectOnlyRadioBtnHeight (17)
/**/
#define NewCaptureClientRectOnlyRadioBtnX (SourceWndEditX + NewSourceWndEditWidth + 22/*gap*/)

#define CaptureEntireWindowRadioBtnX (CaptureClientRectOnlyRadioBtnX)
#define CaptureEntireWindowRadioBtnY (CaptureClientRectOnlyRadioBtnY + CaptureClientRectOnlyRadioBtnHeight + 3/*gap*/)
#define CaptureEntireWindowRadioBtnWidth (160)
#define CaptureEntireWindowRadioBtnHeight (17)
/**/
#define NewCaptureEntireWindowRadioBtnX (NewCaptureClientRectOnlyRadioBtnX)

/* Buttons */
#define SettingsApplyButtonX (CaptureClientRectOnlyRadioBtnX + CaptureClientRectOnlyRadioBtnWidth + 14/*gap*/)
#define SettingsApplyButtonY (CaptureClientRectOnlyRadioBtnY)
#define SettingsApplyButtonWidth (70)
#define SettingsApplyButtonHeight (37)
/**/
#define NewSettingsApplyButtonX (NewCaptureClientRectOnlyRadioBtnX + CaptureClientRectOnlyRadioBtnWidth + 14/*gap*/)


UI::UI(const char *app_name, const char *app_version)
{
    /* Create the main window */
    _wnd = std::unique_ptr<MainWindow>(new MainWindow(
        app_name,
        { CW_USEDEFAULT, CW_USEDEFAULT },
        { MainWndWidth, MainWndHeight },
        IDI_ICON1));
    /* Setup the main window */
    _wnd->SetWndText(std::string(app_name + std::string(" ") + app_version).c_str());
    _wnd->EnableControlsDialogMessages(true);

    /* Create a preview window */
    _preview_wnd = std::unique_ptr<PreviewWindow>(new PreviewWindow(
        { PreviewWndX, PreviewWndY },
        { PreviewWndWidth, PreviewWndHeight }));

    _preview_wnd->ShowWnd(false);
    _preview_wnd->SetWndParent(_wnd.get());
    
    /* Create window controls */
    /* Labels */
    _source_wnd_label = std::unique_ptr<Label>(new Label(
        "Captured window: ",
        { SourceWndLabelX, SourceWndLabelY },
        { SourceWndLabelWidth, SourceWndLabelHeight }));

    _resolution_label = std::unique_ptr<Label>(new Label(
        "Video resolution: ",
        { ResolutionLabelX, ResolutionLabelY },
        { ResolutionLabelWidth, ResolutionLabelHeight }));

    _x_symbol_label = std::unique_ptr<Label>(new Label(
        "X",
        { XSymbolLabelX, XSymbolLabelY },
        { XSymbolLabelWidth, XSymbolLabelHeight }));

    _fps_label = std::unique_ptr<Label>(new Label(
        "Video FPS: ",
        { FpsLabelX, FpsLabelY },
        { FpsLabelWidth, FpsLabelHeight }));

    /* Edits */
    _source_wnd_edit = std::unique_ptr<Edit>(new Edit(
        { SourceWndEditX, SourceWndEditY },
        { SourceWndEditWidth, SourceWndEditHeight }));

    _width_edit = std::unique_ptr<Edit>(new Edit(
        { WidthEditX, WidthEditY },
        { WidthEditWidth, WidthEditHeight }));

    _height_edit = std::unique_ptr<Edit>(new Edit(
        { HeightEditX, HeightEditY },
        { HeightEditWidth, HeightEditHeight }));

    _fps_edit = std::unique_ptr<Edit>(new Edit(
        { FpsEditX, FpsEditY },
        { FpsEditWidth, FpsEditHeight }));

    /* Checkboxes */
    _capture_cursor_checkbox = std::unique_ptr<CheckBox>(new CheckBox(
        "Capture cursor",
        { CaptureCursorCheckboxX, CaptureCursorCheckboxY },
        { CaptureCursorCheckboxWidth, CaptureCursorCheckboxHeight }));

    _capture_optimization_checkbox = std::unique_ptr<CheckBox>(new CheckBox(
        "Optimization",
        { CaptureOptimizationCheckboxX, CaptureOptimizationCheckboxY },
        { CaptureOptimizationCheckboxWidth, CaptureOptimizationCheckboxHeight }));

    /* Radio Buttons */
    _capture_client_rect_only_radio_btn = std::unique_ptr<RadioButton>(new RadioButton(
        "Capture client region only", true,
        { CaptureClientRectOnlyRadioBtnX, CaptureClientRectOnlyRadioBtnY },
        { CaptureClientRectOnlyRadioBtnWidth, CaptureClientRectOnlyRadioBtnHeight }));

    _capture_entire_window_radio_btn = std::unique_ptr<RadioButton>(new RadioButton(
        "Capture entire window", false,
        { CaptureEntireWindowRadioBtnX, CaptureEntireWindowRadioBtnY },
        { CaptureEntireWindowRadioBtnWidth, CaptureEntireWindowRadioBtnHeight }));

    /* Buttons */
    _settings_apply_button = std::unique_ptr<Button>(new Button(
        "Apply",
        { SettingsApplyButtonX, SettingsApplyButtonY },
        { SettingsApplyButtonWidth, SettingsApplyButtonHeight }));

    /* Menus */
    _preview_chekced_menu_point = std::unique_ptr<MenuPoint>(new MenuPoint("Preview", Checked));
    _start_recording_menu_point = std::unique_ptr<MenuPoint>(new MenuPoint("Start recording (key: Alt + K)"));
    _stop_recording_menu_point = std::unique_ptr<MenuPoint>(new MenuPoint("Stop recording (key: Alt + K)"));

    _options_menu = std::unique_ptr<PopupMenu>(new PopupMenu("Options"));
    _wnd_menu = std::unique_ptr<Menu>(new Menu());

    /* Setup the menu points relations */
    _options_menu->AttachMenuPoint(_preview_chekced_menu_point.get());
    _options_menu->AppendSeparator();
    _options_menu->AttachMenuPoint(_start_recording_menu_point.get());
    _options_menu->AttachMenuPoint(_stop_recording_menu_point.get());

    _wnd_menu->AttachPopupMenu(_options_menu.get());

    /* Attach controls to window */
    _wnd->AttachChildControl(_source_wnd_label.get());
    _wnd->AttachChildControl(_resolution_label.get());
    _wnd->AttachChildControl(_x_symbol_label.get());
    _wnd->AttachChildControl(_fps_label.get());

    _wnd->AttachChildControl(_source_wnd_edit.get());
    _wnd->AttachChildControl(_width_edit.get());
    _wnd->AttachChildControl(_height_edit.get());
    _wnd->AttachChildControl(_fps_edit.get());

    _wnd->AttachChildControl(_capture_cursor_checkbox.get());

    _wnd->AttachChildControl(_capture_client_rect_only_radio_btn.get());
    _wnd->AttachChildControl(_capture_entire_window_radio_btn.get());

    _wnd->AttachChildControl(_capture_optimization_checkbox.get());

    _wnd->AttachChildControl(_settings_apply_button.get());

    _wnd->AttachMenu(_wnd_menu.get());

    /* Normal font */
    _normal_font = std::unique_ptr<NormalFont>(new NormalFont());
    _normal_font->SetFont(_source_wnd_label.get());
    _normal_font->SetFont(_resolution_label.get());
    _normal_font->SetFont(_x_symbol_label.get());
    _normal_font->SetFont(_fps_label.get());

    _normal_font->SetFont(_source_wnd_edit.get());
    _normal_font->SetFont(_width_edit.get());
    _normal_font->SetFont(_height_edit.get());
    _normal_font->SetFont(_fps_edit.get());

    _normal_font->SetFont(_capture_optimization_checkbox.get());
    _normal_font->SetFont(_capture_cursor_checkbox.get());

    _normal_font->SetFont(_capture_client_rect_only_radio_btn.get());
    _normal_font->SetFont(_capture_entire_window_radio_btn.get());

    _normal_font->SetFont(_settings_apply_button.get());

    /* Transform callbacks */
#undef NEW_PREVIEW_WIDTH
#define NEW_PREVIEW_WIDTH (parent_wnd->GetWndClientSize().first - (2 * 10)/*gap*/)

    /* The main window sides relstion callback (will be attached to last label parent resize callback) */
    auto main_wnd_sides_callback = [this](void *ptr)->void {
        GetMiscForParentResize(ptr);

        if (_preview_chekced_menu_point->GetState())
        {
            int preview_wnd_client_bottom_pos = _preview_wnd->GetWndPos().second + _preview_wnd->GetWndSize().second;
            if (parent_wnd->GetWndSize().second - preview_wnd_client_bottom_pos != 12)
            {
                parent_wnd->SetWndSize({ new_parent_wnd_size.first, NewMainWndHeightFull });
            }
        }
        else
        {
            if (parent_wnd->GetWndClientSize().second != 77)
            {
                parent_wnd->SetWndSize({ new_parent_wnd_size.first, NewMainWndHeightShort });
            }
        }
        
        };

    auto preview_wnd_resize_callback = [this](void *ptr)->void {
        GetMiscForParentResize(ptr);

        _preview_wnd->SetWndSize({ NewPreviewWndWidth, NewPreviewWndHeight });
        };


    /* Labels */
    _source_wnd_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        label->UpdateWnd();
        });

    _resolution_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        label->UpdateWnd();
        });

    _x_symbol_label->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        label->SetWndPos({ NewXSymbolLabelX, XSymbolLabelY });
        });

    _fps_label->AddCallback("ParentResizeCallback", [this, preview_wnd_resize_callback, main_wnd_sides_callback](void *ptr)->void {
        Label *label = GetControlForParentResize(Label, ptr);
        GetMiscForParentResize(ptr);

        label->UpdateWnd();

         /* Preview window resize */
        preview_wnd_resize_callback(ptr);

        /* Window's sides proportion */
        main_wnd_sides_callback(ptr);
        });


    /* Edits */
    _source_wnd_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Edit *edit = GetControlForParentResize(Edit, ptr);
        GetMiscForParentResize(ptr);

        edit->SetWndSize({ NewSourceWndEditWidth, SourceWndEditHeight });
        });

    _width_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Edit *edit = GetControlForParentResize(Edit, ptr);
        GetMiscForParentResize(ptr);

        edit->SetWndSize({ NewWidthEditWidth, WidthEditHeight });
        });

    _height_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Edit *edit = GetControlForParentResize(Edit, ptr);
        GetMiscForParentResize(ptr);

        edit->SetWndPos({ NewHeightEditX, HeightEditY });
        edit->SetWndSize({ NewHeightEditWidth, HeightEditHeight });
        });

    _fps_edit->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Edit *edit = GetControlForParentResize(Edit, ptr);
        GetMiscForParentResize(ptr);

        edit->UpdateWnd();
        });


    /* Checkboxes */
    _capture_cursor_checkbox->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        CheckBox *button = GetControlForParentResize(CheckBox, ptr);
        GetMiscForParentResize(ptr);

        button->UpdateWnd();
        });

    _capture_optimization_checkbox->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        CheckBox *button = GetControlForParentResize(CheckBox, ptr);
        GetMiscForParentResize(ptr);

        button->SetWndPos({ NewCaptureOptimizationCheckboxX, CaptureOptimizationCheckboxY });
        });


    /* Radio Buttons */
    _capture_client_rect_only_radio_btn->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Button *button = GetControlForParentResize(Button, ptr);
        GetMiscForParentResize(ptr);

        button->SetWndPos({ NewCaptureClientRectOnlyRadioBtnX, CaptureClientRectOnlyRadioBtnY });
        });

    _capture_entire_window_radio_btn->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Button *button = GetControlForParentResize(Button, ptr);
        GetMiscForParentResize(ptr);

        button->SetWndPos({ NewCaptureEntireWindowRadioBtnX, CaptureEntireWindowRadioBtnY });
        });


    /* Buttons */
    _settings_apply_button->AddCallback("ParentResizeCallback", [this](void *ptr)->void {
        Button *button = GetControlForParentResize(Button, ptr);
        GetMiscForParentResize(ptr);

        button->SetWndPos({ NewSettingsApplyButtonX, SettingsApplyButtonY });
        });
}
