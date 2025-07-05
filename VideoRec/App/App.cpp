#include "App.h"

App::App(const char *app_name, const char *app_version)
{
    /* Create app model & UI */
    _model = std::make_unique<Model>();
    _ui = std::make_unique<UI>(app_name, app_version);

    /* Default controls' settings */
    _ui->get_capture_optimization_checkbox()->SetState(true);
    _ui->get_capture_optimization_checkbox()->SetInputState(false);
    _ui->get_capture_cursor_checkbox()->SetState(true);
    _ui->get_fps_edit()->SetWndText("30");
    _ui->get_capture_client_rect_only_radio_btn()->SetState(true);
    _ui->get_settings_apply_button()->SetInputState(false);
    _ui->get_stop_recording_menu_point()->SetState(false);

    /* Set source */
    _model->get_video_rec()->SetNewSource(nullptr);
    /* Start idle mode */
    _model->get_video_rec()->StartIdleMode();
    /* Set width & height edit fields */
    _ui->get_width_edit()->SetWndText(std::to_string(_model->get_video_rec()->GetSrcSize().first).c_str());
    _ui->get_height_edit()->SetWndText(std::to_string(_model->get_video_rec()->GetSrcSize().second).c_str());
    /* Start preview */
    _ui->get_preview_wnd()->SetPreview(_model->get_video_rec()->GetPreview());
    _ui->get_preview_wnd()->ShowWnd(true);

    /* Add async loop callback for main window */
    _ui->get_wnd()->AddCallback("AsyncLoopCallback", [this](void *ptr)->void {
        Window *wnd = GetControl(Window, ptr);

        static std::chrono::time_point<std::chrono::steady_clock> millis_timer = std::chrono::steady_clock::now();
        static bool key_tmp_state_flag = true;

        /* Alt + K combination for start/stop recording */
        if (GetAsyncKeyState('K') < 0 && GetAsyncKeyState(VK_MENU) < 0 && key_tmp_state_flag)
        {
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - millis_timer).count() >= 1000)
            {
                millis_timer = std::chrono::steady_clock::now();
                if (!_ui->get_stop_recording_menu_point()->GetState())
                {
                    _ui->get_start_recording_menu_point()->operator()("MainCallback", (void *)_ui->get_start_recording_menu_point().get());
                }
                else if (_ui->get_stop_recording_menu_point()->GetState())
                {
                    _ui->get_stop_recording_menu_point()->operator()("MainCallback", (void *)_ui->get_stop_recording_menu_point().get());
                }

                key_tmp_state_flag = false;
            }
        }
        else if (!GetAsyncKeyState('K') && !key_tmp_state_flag)
        {
            key_tmp_state_flag = true;
        }
        });

    /* Add controls' callbacks */

    /* Edits' callbacks */
    _ui->get_source_wnd_edit()->AddCallback("MainCallback", [this](void *ptr)->void {
        //Edit *edit = GetControl(Edit, ptr);

        _model->get_source_wnd_changed_flag() = true;
        _ui->get_settings_apply_button()->SetInputState(true);
        });

    _ui->get_width_edit()->AddCallback("MainCallback", [this](void *ptr)->void {
        //Edit *edit = GetControl(Edit, ptr);

        _ui->get_settings_apply_button()->SetInputState(true);
        });

    _ui->get_height_edit()->AddCallback("MainCallback", [this](void *ptr)->void {
        //Edit *edit = GetControl(Edit, ptr);

        _ui->get_settings_apply_button()->SetInputState(true);
        });

    /* Checkboxes' callbacks */
    _ui->get_capture_optimization_checkbox()->AddCallback("MainCallback", [this](void *ptr)->void {
        CheckBox *button = GetControl(CheckBox, ptr);

        _model->get_capture_optimization_flag() = !_model->get_capture_optimization_flag();

        button->SetState(_model->get_capture_optimization_flag());
        _ui->get_settings_apply_button()->SetInputState(true);
        });

    _ui->get_capture_cursor_checkbox()->AddCallback("MainCallback", [this](void *ptr)->void {
        CheckBox *button = GetControl(CheckBox, ptr);

        _model->get_capture_cursor_flag() = !_model->get_capture_cursor_flag();

        button->SetState(_model->get_capture_cursor_flag());
        _ui->get_settings_apply_button()->SetInputState(true);
        });

    /* Radio Buttons' callbacks */
    _ui->get_capture_client_rect_only_radio_btn()->AddCallback("MainCallback", [this](void *ptr)->void {
        RadioButton *button = GetControl(RadioButton, ptr);

        /* Prevent false activation */
        if (button->GetState())
        {
            /* Reset optimization checkbox */
            if (!_ui->get_capture_optimization_checkbox()->GetState())
            {
                _ui->get_capture_optimization_checkbox()->operator()("MainCallback", (void *)_ui->get_capture_optimization_checkbox().get());
            }
            _ui->get_capture_optimization_checkbox()->SetInputState(false);

            _model->get_capture_client_rect_only_flag() = true;
            _ui->get_settings_apply_button()->SetInputState(true);
        }
        });

    _ui->get_capture_entire_window_radio_btn()->AddCallback("MainCallback", [this](void *ptr)->void {
        RadioButton *button = GetControl(RadioButton, ptr);

        /* Prevent false activation */
        if (button->GetState())
        {
            /* Allow change optimization checkbox */
            _ui->get_capture_optimization_checkbox()->SetInputState(true);

            _model->get_capture_client_rect_only_flag() = false;
            _ui->get_settings_apply_button()->SetInputState(true);
        }
        });

    /* Buttons' callbacks */
    _ui->get_settings_apply_button()->AddCallback("MainCallback", [this](void *ptr)->void {
        Button *button = GetControl(Button, ptr);

        /* Stop recording */
        _ui->get_stop_recording_menu_point()->operator()("MainCallback", _ui->get_stop_recording_menu_point().get());

        /* Stop idle mode and preview */
        _model->get_video_rec()->StopIdleMode();
        _ui->get_preview_wnd()->DeletePreview();

        try
        {
            /* Set source window and video resolution */
            std::unique_ptr<char[]> wnd_name(_ui->get_source_wnd_edit()->GetWndText(256));
            std::unique_ptr<char[]> video_width(_ui->get_width_edit()->GetWndText(10));
            std::unique_ptr<char[]> video_height(_ui->get_height_edit()->GetWndText(10));

            if (atoi(video_width.get()) == 0 && video_width[0] != '0' && video_width[0] != '\0')
            {
                throw std::string("Video width must be a number!");
            }
            else if (atoi(video_height.get()) == 0 && video_height[0] != '0' && video_height[0] != '\0')
            {
                throw std::string("Video height must be a number!");
            }

            _model->get_video_rec()->ApplyFlags();

            _model->get_video_rec()->SetNewSource(
                (wnd_name[0] == '\0' ? nullptr : (const char *)wnd_name.get()),
                { (video_width[0] == '\0' || video_width[0] == '0' || _model->get_source_wnd_changed_flag() ? -1 : atoi(video_width.get())),
                (video_height[0] == '\0' || video_height[0] == '0' || _model->get_source_wnd_changed_flag() ? -1 : atoi(video_height.get())) });

            if (_model->get_source_wnd_changed_flag() || video_width[0] == '\0' || video_width[0] == '0' || video_height[0] == '\0' || video_height[0] == '0')
            {
                _ui->get_width_edit()->SetWndText(std::to_string(_model->get_video_rec()->GetSrcSize().first).c_str());
                _ui->get_height_edit()->SetWndText(std::to_string(_model->get_video_rec()->GetSrcSize().second).c_str());
            }

            _model->get_source_wnd_changed_flag() = false;
        }
        catch (const std::string &error)
        {
            std::unique_ptr<wchar_t[]> w_error(to_utf16(error.c_str()));
            MessageBoxW(NULL, w_error.get(), L"Error", MB_OK);
        
            return;
        }
    
        if (_model->get_allow_preview_flag())
        {
            /* Set preview and start idle mode */
            _ui->get_preview_wnd()->SetPreview(_model->get_video_rec()->GetPreview());
            _model->get_video_rec()->StartIdleMode();
        }        

        button->SetInputState(false);
        });


    /* Menu points' callbacks */
    _ui->get_preview_chekced_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        _model->get_allow_preview_flag() = !_model->get_allow_preview_flag();
        menu_point->SetState(_model->get_allow_preview_flag());

        if (_model->get_allow_preview_flag())
        {
            /* Set preview */
            _ui->get_preview_wnd()->SetPreview(_model->get_video_rec()->GetPreview());

            /* Update all sizes of the windows */
            _ui->get_wnd()->SendMsg(WM_SIZE,
                SIZE_RESTORED,
                ((0xffff & _ui->get_wnd()->GetWndSize().second) << 16) + (0xffff & _ui->get_wnd()->GetWndSize().first));

            /* Start idle mode (saves recording settings if record in process) */
            _model->get_video_rec()->StartIdleMode();

            /* Delay for first frame creation */
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            _ui->get_preview_wnd()->ShowWnd(true);

        }
        else if (!_model->get_allow_preview_flag())
        {
            _ui->get_preview_wnd()->DeletePreview();
            _ui->get_preview_wnd()->ShowWnd(false);
            /* Update all sizes of the windows */
            _ui->get_wnd()->SendMsg(WM_SIZE,
                SIZE_RESTORED,
                ((0xffff & _ui->get_wnd()->GetWndSize().second) << 16) + (0xffff & _ui->get_wnd()->GetWndSize().first));

            /* Stop idle mode (continue recording) */
            _model->get_video_rec()->StopIdleMode();
        }
        });

    _ui->get_start_recording_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        try
        {
            std::unique_ptr<char[]> video_fps(_ui->get_fps_edit()->GetWndText(5));

            if (atoi(video_fps.get()) == 0 && video_fps[0] != '\0' && video_fps[0] != '0')
            {
                throw std::string("Video FPS must be a number!");
            }
            else if (video_fps[0] == '\0' || video_fps[0] == '0')
            {
                _ui->get_fps_edit()->SetWndText("30");
                video_fps = std::unique_ptr<char[]>(new char[] { "30" });
            }

            _model->get_video_rec()->StartRecording(_model->get_file_name_generator()->CreateFileName(), atoi(video_fps.get()));

            menu_point->SetState(false);
            _ui->get_stop_recording_menu_point()->SetState(true);
        }
        catch (const std::string &error)
        {
            std::unique_ptr<wchar_t[]> w_error(to_utf16(error.c_str()));
            MessageBoxW(NULL, w_error.get(), L"Error", MB_OK);

            _ui->get_stop_recording_menu_point()->operator()("MainCallback", _ui->get_stop_recording_menu_point().get());
        }
        });

    _ui->get_stop_recording_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        _model->get_video_rec()->StopRecording();
        if (_model->get_allow_preview_flag())
        {
            _model->get_video_rec()->StartIdleMode();
        }

        menu_point->SetState(false);
        _ui->get_start_recording_menu_point()->SetState(true);
        });

}

App::~App()
{
    _ui.reset();
    _model.reset();
}

int App::Run()
{
    _ui->get_wnd()->ShowWnd(true);
    _ui->get_preview_wnd()->ShowWnd(true);
    return _ui->get_wnd()->Run();
}
