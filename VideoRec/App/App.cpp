#include "App.h"

App::App(const char *app_name, const char *app_version)
{
    /* Create app model & UI */
    _model = new Model();
    _ui = new UI(app_name, app_version);

    /* Add init settings */
    _ui->get_video_sources_list()->AddItem("Capture a window");

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
    _ui->get_video_sources_list()->AddCallback("MainCallback", [this](void *ptr)->void {
        ComboBox *list = GetControl(ComboBox, ptr);

        /* Reset controls' state */
        _ui->ResetVideoRecordingRelatedControls();
        _model->ResetAll();

        if (list->GetItemId() == 1/* "Capture a window" */)
        {
            /* Show needed controls and preview window if allows */
            _ui->get_preview_wnd()->ShowWnd(_model->get_allow_preview_flag());

            _ui->get_video_source_wnd_label()->ShowWnd(true);
            _ui->get_video_source_wnd_edit()->ShowWnd(true);
            _ui->get_video_resolution_label()->ShowWnd(true);
            _ui->get_video_resolution_x_symbol_label()->ShowWnd(true);
            _ui->get_video_width_edit()->ShowWnd(true);
            _ui->get_video_height_edit()->ShowWnd(true);
            
            /* Init VideoRec module */
            _model->get_video_rec() = new VideoRecorder(_model->get_allow_preview_flag());

            /* Set source window and video resolution */
            char wnd_name[65] = { 0 };
            _ui->get_video_source_wnd_edit()->GetWndText(wnd_name, 64);

            char wnd_width[6] = { 0 };
            char wnd_height[6] = { 0 };
            _ui->get_video_width_edit()->GetWndText(wnd_width, 5);
            _ui->get_video_height_edit()->GetWndText(wnd_height, 5);

            try
            {
                _model->get_video_rec()->SetNewSource(
                    (wnd_name[0] == '\0' ? nullptr : wnd_name),
                    (wnd_width[0] == '\0' ? -1 : atoi(wnd_width)),
                    (wnd_height[0] == '\0' ? -1 : atoi(wnd_height)));
            }
            catch (std::string error)
            {
                int str_size = (int)error.length() + 1;
                SmtObj<wchar_t[]> w_error = new wchar_t[str_size] { 0 };
                MultiByteToWideChar(CP_UTF8, 0, error.c_str(), str_size, w_error, str_size);

                MessageBoxW(NULL, w_error, L"Error", MB_OK);

                return;
            }

            /* Set context for preview */
            _ui->get_preview_wnd()->SetSrc(_model->get_video_rec()->GetPreviewContext(), _model->get_video_rec()->GetSrcWidth(), _model->get_video_rec()->GetSrcHeight());
        }

        });

    _ui->get_video_codecs_list()->AddCallback("MainCallback", [this](void *ptr)->void {
        ComboBox *list = GetControl(ComboBox, ptr);

        });

    _ui->get_audio_sources_list()->AddCallback("MainCallback", [this](void *ptr)->void {
        ComboBox *list = GetControl(ComboBox, ptr);

        });

    _ui->get_audio_codecs_list()->AddCallback("MainCallback", [this](void *ptr)->void {
        ComboBox *list = GetControl(ComboBox, ptr);

        });

    /* Menu points' callbacks */
    _ui->get_preview_chekced_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        _model->get_allow_preview_flag() = !_model->get_allow_preview_flag();
        menu_point->SetState(_model->get_allow_preview_flag());

        /* Call select source callback to reload preview flag for modules */
        _ui->get_video_sources_list()->operator()("MainCallback", (void *)_ui->get_video_sources_list());
        });

    _ui->get_start_recording_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        if (_model->get_video_rec())
        {
            int fps = 60;
            _model->get_video_rec()->StartRecording((const char *)_model->get_file_name_generator()->CreateFileName(), fps);

        }

        menu_point->SetState(false);
        _ui->get_stop_recording_menu_point()->SetState(true);
        });

    _ui->get_stop_recording_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        if (_model->get_video_rec())
        {
            _model->get_video_rec()->StopRecording();
        }

        menu_point->SetState(false);
        _ui->get_start_recording_menu_point()->SetState(true);
        });

}

int App::Run()
{
    _ui->get_wnd()->ShowWnd(true);
    return _ui->get_wnd()->Run();
}
