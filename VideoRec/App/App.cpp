#include "App.h"

App::App(const char *app_name, const char *app_version)
{
    /* Create app model & UI */
    _model = new Model();
    _ui = new UI(app_name, app_version);

    /* Set source */
    _model->get_video_rec()->SetNewSource(nullptr);
    _ui->get_preview_wnd()->SetSrc(_model->get_video_rec()->GetPreviewContext(), _model->get_video_rec()->GetSrcWidth(), _model->get_video_rec()->GetSrcHeight());
    _ui->get_preview_wnd()->ShowWnd(true);

    /* Add async loop callback for main window */
    _ui->get_wnd()->AddCallback("AsyncLoopCallback", [this](void *ptr)->void {
        Window *wnd = GetControl(Window, ptr);

        static std::chrono::time_point<std::chrono::steady_clock> millis_timer = std::chrono::steady_clock::now();
        static bool key_tmp_state_flag = true;

        if (GetAsyncKeyState('K') < 0 && GetAsyncKeyState(VK_MENU) < 0 && key_tmp_state_flag)
        {
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - millis_timer).count() >= 1000)
            {
                millis_timer = std::chrono::steady_clock::now();
                if (!_ui->get_stop_recording_menu_point()->GetState())
                {
                    _ui->get_start_recording_menu_point()->operator()("MainCallback", (void *)_ui->get_start_recording_menu_point());
                }
                else if (_ui->get_stop_recording_menu_point()->GetState())
                {
                    _ui->get_stop_recording_menu_point()->operator()("MainCallback", (void *)_ui->get_stop_recording_menu_point());
                }

                key_tmp_state_flag = false;
            }
        }
        else if (!GetAsyncKeyState('K') && !key_tmp_state_flag)
        {
            key_tmp_state_flag = true;
        }
        else if (GetAsyncKeyState(VK_ESCAPE) != 0 && GetForegroundWindow() == wnd->GetHwnd())
        {
            DestroyWindow(wnd->GetHwnd());
        }
        });

    /* Add controls' callbacks */

    /* Edits' callbacks */
    _ui->get_video_source_wnd_edit()->AddCallback("MainCallback", [this](void *ptr)->void {
        //Edit *edit = GetControl(Edit, ptr);

        _ui->get_video_settings_apply_button()->ShowWnd(true);
        });

    _ui->get_video_width_edit()->AddCallback("MainCallback", [this](void *ptr)->void {
        //Edit *edit = GetControl(Edit, ptr);

        _ui->get_video_settings_apply_button()->ShowWnd(true);
        });

    _ui->get_video_height_edit()->AddCallback("MainCallback", [this](void *ptr)->void {
        //Edit *edit = GetControl(Edit, ptr);

        _ui->get_video_settings_apply_button()->ShowWnd(true);
        });


    /* Buttons' callbacks */
    _ui->get_video_settings_apply_button()->AddCallback("MainCallback", [this](void *ptr)->void {
        Button *button = GetControl(Button, ptr);

        /* Stop recording */
        _ui->get_stop_recording_menu_point()->operator()("MainCallback", _ui->get_stop_recording_menu_point());
    
        try
        {
            /* Set source window and video resolution */
            char wnd_name[65] = { 0 };
            _ui->get_video_source_wnd_edit()->GetWndText(wnd_name, 64);
        
            char video_width[6] = { 0 };
            char video_height[6] = { 0 };
            _ui->get_video_width_edit()->GetWndText(video_width, 5);
            _ui->get_video_height_edit()->GetWndText(video_height, 5);
            if (atoi(video_width) == 0 && video_width[0] != '0' && video_width[0] != '\0')
            {
                throw std::string("Video width must be a number!");
            }
            else if (atoi(video_height) == 0 && video_height[0] != '0' && video_height[0] != '\0')
            {
                throw std::string("Video height must be a number!");
            }
        
            _model->get_video_rec()->SetNewSource(
                (wnd_name[0] == '\0' ? nullptr : wnd_name),
                (video_width[0] == '\0' ? -1 : atoi(video_width)),
                (video_height[0] == '\0' ? -1 : atoi(video_height)));
        }
        catch (std::string error)
        {
            int str_size = (int)error.length() + 1;
            SmtObj<wchar_t[]> w_error = new wchar_t[str_size] { 0 };
            MultiByteToWideChar(CP_UTF8, 0, error.c_str(), str_size, w_error, str_size);
        
            MessageBoxW(NULL, w_error, L"Error", MB_OK);
        
            return;
        }
    
        if (_model->get_allow_preview_flag())
        {
            /* Set preview context */
            _ui->get_preview_wnd()->SetSrc(_model->get_video_rec()->GetPreviewContext(), _model->get_video_rec()->GetSrcWidth(), _model->get_video_rec()->GetSrcHeight());
            /* Stop recording (need restart for idle mode activation for preview) */
            _ui->get_stop_recording_menu_point()->operator()("MainCallback", _ui->get_stop_recording_menu_point());
        }

        button->ShowWnd(false);
        });


    /* Menu points' callbacks */
    _ui->get_preview_chekced_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        _model->get_allow_preview_flag() = !_model->get_allow_preview_flag();
        menu_point->SetState(_model->get_allow_preview_flag());

        if (_model->get_allow_preview_flag())
        {
            _ui->get_preview_wnd()->SetSrc(_model->get_video_rec()->GetPreviewContext(), _model->get_video_rec()->GetSrcWidth(), _model->get_video_rec()->GetSrcHeight());
            _ui->get_preview_wnd()->ShowWnd(true);
            /* Stop recording (need restart for idle mode activation for preview) */
            _ui->get_stop_recording_menu_point()->operator()("MainCallback", _ui->get_stop_recording_menu_point());
        }
        else if (!_model->get_allow_preview_flag())
        {
            _ui->get_preview_wnd()->SetSrc(nullptr, 0, 0);
            _ui->get_preview_wnd()->ShowWnd(false);
            /* Stop recording (need restart for idle mode deactivation for preview) */
            _ui->get_stop_recording_menu_point()->operator()("MainCallback", _ui->get_stop_recording_menu_point());
        }
        });

    _ui->get_start_recording_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        try
        {
            char video_fps[6] = { 0 };
            _ui->get_video_fps_edit()->GetWndText(video_fps, 5);
            if (video_fps[0] == '\0')
            {
                throw std::string("FPS is not set!");
            }
            else if (atoi(video_fps) == 0 && video_fps[0] != '0')
            {
                throw std::string("Video FPS must be a number!");
            }

            _model->get_video_rec()->StartRecording((const char *)_model->get_file_name_generator()->CreateFileName(), atoi(video_fps));

            menu_point->SetState(false);
            _ui->get_stop_recording_menu_point()->SetState(true);
        }
        catch (const std::string &error)
        {
            int str_size = (int)error.length() + 1;
            SmtObj<wchar_t[]> w_error = new wchar_t[str_size] { 0 };
            MultiByteToWideChar(CP_UTF8, 0, error.c_str(), str_size, w_error, str_size);

            MessageBoxW(NULL, w_error, L"Error", MB_OK);
        }
        });

    _ui->get_stop_recording_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        _model->get_video_rec()->StopRecording();

        menu_point->SetState(false);
        _ui->get_start_recording_menu_point()->SetState(true);
        });

}

int App::Run()
{
    _ui->get_wnd()->ShowWnd(true);
    return _ui->get_wnd()->Run();
}
