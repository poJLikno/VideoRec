#include "App.h"

App::App(const char *app_name, const char *app_version)
{
    /* Create app model & UI */
    _model = new Model();
    _ui = new UI(app_name, app_version);

    /* Add init settings */
    _model->get_dshow_rec() = new DShowRecorder(_model->get_allow_preview_flag());
    _model->get_dshow_rec()->AttachDShowWindow(_ui->get_dshow_wnd());

    /* Add devices to list */
    auto _add_devices_to_list_func = [](Device *device, SmtObj<ComboBox> &combo_box)->void {
        wchar_t **device_names = nullptr;
        unsigned int names_num = 0u;
        device->GetDeviceList(&device_names, &names_num);
        if (device_names && names_num) {
            for (unsigned int i = 0u; i < names_num; ++i) {
                /* wchar_t support */
                combo_box->SendMsg(CB_ADDSTRING, NULL, (LPARAM)device_names[i]);;
                free(device_names[i]);
            }
            free(device_names);
            device_names = nullptr;
        }
        };

    /* Add VideoRec module */
    _ui->get_video_sources_list()->AddItem("Capture a window");
    /* Add DShow module */
    _add_devices_to_list_func(_model->get_dshow_rec()->GetVideoDevice(), _ui->get_video_sources_list());
    _add_devices_to_list_func(_model->get_dshow_rec()->GetVideoCodec(), _ui->get_video_codecs_list());
    _add_devices_to_list_func(_model->get_dshow_rec()->GetAudioDevice(), _ui->get_audio_sources_list());
    _add_devices_to_list_func(_model->get_dshow_rec()->GetAudioCodec(), _ui->get_audio_codecs_list());

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
        _model->ResetAllVideo();

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
            _ui->get_video_fps_label()->ShowWnd(true);
            _ui->get_video_fps_edit()->ShowWnd(true);
            
            /* Init VideoRec module */
            _model->get_video_rec() = new VideoRecorder(_model->get_allow_preview_flag());

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

            /* Set context for preview */
            _ui->get_preview_wnd()->SetSrc(_model->get_video_rec()->GetPreviewContext(), _model->get_video_rec()->GetSrcWidth(), _model->get_video_rec()->GetSrcHeight());
        }
        else if (list->GetItemId() != 0/* "Capture DShow" */)
        {
            /* Show needed controls and preview window if allows */
            _ui->get_video_codec_label()->ShowWnd(true);
            _ui->get_video_codecs_list()->ShowWnd(true);

            /* Set device */
            /* DShow always initialazed */
            _model->get_dshow_rec()->SelectDevice(list->GetItemId() - 2, _model->get_dshow_rec()->GetVideoDevice());

            // Check filter settings access
            if (_model->get_dshow_rec()->HasFilterSettings(_model->get_dshow_rec()->GetVideoDevice()))
            {
                // Set state for ui
                _ui->get_video_source_menu_point()->SetState(true);
            }

            // Check pin settings access
            if (_model->get_dshow_rec()->HasPinSettings(_model->get_dshow_rec()->GetVideoDevice()))
            {
                // Set state for ui
                _ui->get_video_capture_pin_menu_point()->SetState(true);
            }

            /* Show codec controls */
            _ui->get_video_codecs_list()->operator()("MainCallback", (void *)_ui->get_video_codecs_list());
        }
        });

    _ui->get_video_codecs_list()->AddCallback("MainCallback", [this](void *ptr)->void {
        ComboBox *list = GetControl(ComboBox, ptr);

        // Disable recording (if need)
        _ui->get_stop_recording_menu_point()->operator()("MainCallback", (void *)_ui->get_stop_recording_menu_point());

        _model->get_dshow_rec()->GetVideoCodec()->ReleaseDevice();
        _model->get_dshow_rec()->SelectDevice(list->GetItemId() - 1, _model->get_dshow_rec()->GetVideoCodec());

        double default_cuality = _model->get_dshow_rec()->GetDefaultVideoCodecQuality();
        if (default_cuality >= 0.0) {
            _ui->get_video_quality_edit()->SetWndText(std::to_string((int)round(default_cuality * 100.0)).c_str());
            _ui->get_video_quality_label()->ShowWnd(true);
            _ui->get_video_quality_percent_label()->ShowWnd(true);
            _ui->get_video_quality_edit()->ShowWnd(true);
        }
        else {
            _ui->get_video_quality_label()->ShowWnd(false);
            _ui->get_video_quality_percent_label()->ShowWnd(false);
            _ui->get_video_quality_edit()->ShowWnd(false);
        }        
        });

    _ui->get_audio_sources_list()->AddCallback("MainCallback", [this](void *ptr)->void {
        ComboBox *list = GetControl(ComboBox, ptr);

        // Disable recording (if need)
        _ui->get_stop_recording_menu_point()->operator()("MainCallback", (void *)_ui->get_stop_recording_menu_point());
        _model->get_dshow_rec()->GetAudioDevice()->ReleaseDevice();

        /* Reset audio UI */
        _ui->get_audio_source_menu_point()->SetState(false);
        _ui->get_audio_capture_pin_menu_point()->SetState(false);

        /* Set device */
        /* DShow always initialazed */
        _model->get_dshow_rec()->SelectDevice(list->GetItemId() - 1, _model->get_dshow_rec()->GetAudioDevice());

        // Check filter settings access
        if (_model->get_dshow_rec()->HasFilterSettings(_model->get_dshow_rec()->GetAudioDevice()))
        {
            // Set state for ui
            _ui->get_audio_source_menu_point()->SetState(true);
        }

        // Check pin settings access
        if (_model->get_dshow_rec()->HasPinSettings(_model->get_dshow_rec()->GetAudioDevice()))
        {
            // Set state for ui
            _ui->get_audio_capture_pin_menu_point()->SetState(true);
        }        
        });

    _ui->get_audio_codecs_list()->AddCallback("MainCallback", [this](void *ptr)->void {
        ComboBox *list = GetControl(ComboBox, ptr);

        // Disable recording (if need)
        _ui->get_stop_recording_menu_point()->operator()("MainCallback", (void *)_ui->get_stop_recording_menu_point());

        _model->get_dshow_rec()->GetAudioCodec()->ReleaseDevice();
        _model->get_dshow_rec()->SelectDevice(list->GetItemId() - 1, _model->get_dshow_rec()->GetAudioCodec());
        });

    /* Menu points' callbacks */
    _ui->get_preview_chekced_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        _model->get_allow_preview_flag() = !_model->get_allow_preview_flag();
        menu_point->SetState(_model->get_allow_preview_flag());

        /* Call select source callback to reload preview flag for VideoRec module */
        if (_model->get_video_rec())
        {
            _ui->get_video_sources_list()->operator()("MainCallback", (void *)_ui->get_video_sources_list());
        }
        /* For DShow enougt just restart graph */
        _model->get_dshow_rec()->StopRecording();
        });

    _ui->get_video_source_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        //MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        // Disable recording (if need)
        _model->get_dshow_rec()->StopRecording();/* if it is executing, VideoRec = nullptr */

        _model->get_dshow_rec()->ShowVideoFilterSettings();
        });

    _ui->get_video_capture_pin_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        //MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        // Disable recording (if need)
        _model->get_dshow_rec()->StopRecording();/* if it is executing, VideoRec = nullptr */

        _model->get_dshow_rec()->ShowVideoPinSettings();
        });

    _ui->get_audio_source_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        //MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        // Disable recording (if need)
        _model->get_dshow_rec()->StopRecording();/* if it is executing, VideoRec = nullptr */

        _model->get_dshow_rec()->ShowAudioFilterSettings();
        });

    _ui->get_audio_capture_pin_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        //MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        // Disable recording (if need)
        _model->get_dshow_rec()->StopRecording();/* if it is executing, VideoRec = nullptr */

        _model->get_dshow_rec()->ShowAudioPinSettings();
        });

    _ui->get_start_recording_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        try
        {
            if (_model->get_video_rec())
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
            }

            /* Also try recording DShow video / audio */
            char video_quality[4] = { 0 };
            _ui->get_video_quality_edit()->GetWndText(video_quality, 3);
            if (video_quality[0] == '\0')
            {
                throw std::string("Video quality is not set!");
            }
            else if (atoi(video_quality) == 0 && video_quality[0] != '0')
            {
                throw std::string("Video quality must be a number [1% - 100%]!");
            }

            _model->get_dshow_rec()->GetVideoCodec()->SetCodecQuality((double)atoi(video_quality) / 100.0);

            _model->get_dshow_rec()->StartRecording();

            
            menu_point->SetState(false);
            _ui->get_stop_recording_menu_point()->SetState(true);
        }
        catch (const std::string &error)
        {
            int str_size = (int)error.length() + 1;
            SmtObj<wchar_t[]> w_error = new wchar_t[str_size] { 0 };
            MultiByteToWideChar(CP_UTF8, 0, error.c_str(), str_size, w_error, str_size);

            MessageBoxW(NULL, w_error, L"Error", MB_OK);

            // Disable recording (if need)
            _ui->get_stop_recording_menu_point()->operator()("MainCallback", (void *)_ui->get_stop_recording_menu_point());
        }
        catch (const std::wstring &message)
        {
            MessageBox(NULL, message.c_str(), L"Attention", MB_OK);

            // Disable recording (if need)
            _ui->get_stop_recording_menu_point()->operator()("MainCallback", (void *)_ui->get_stop_recording_menu_point());
        }
        });

    _ui->get_stop_recording_menu_point()->AddCallback("MainCallback", [this](void *ptr)->void {
        MenuPoint *menu_point = GetControl(MenuPoint, ptr);

        /* VideoRec module */
        if (_model->get_video_rec())
        {
            _model->get_video_rec()->StopRecording();
        }

        /* DShow */
        _model->get_dshow_rec()->StopRecording();
        
        menu_point->SetState(false);
        _ui->get_start_recording_menu_point()->SetState(true);
        });

}

int App::Run()
{
    _ui->get_wnd()->ShowWnd(true);
    return _ui->get_wnd()->Run();
}
