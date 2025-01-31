#include "App.h"

App::App(const char *app_name, const char *app_version, const int &argc, const char **argv)
{
    int dst_video_fps = 30;
    int dst_video_width = -1;
    int dst_video_height = -1;

    /* Get dst viedeo resolution */
    if (argc > 1)
    {
        if (atoi(argv[1]) == 0)
        {
            throw std::string("Failed to get dst video FPS!");
        }

        dst_video_fps = atoi(argv[1]);

        if (argc > 2)
        {
            if (atoi(argv[2]) == 0)
            {
                throw std::string("Failed to get dst video width!");
            }

            dst_video_width = atoi(argv[2]);
        }

        if (argc > 3)
        {
            if (atoi(argv[3]) == 0)
            {
                throw std::string("Failed to get dst video height!");
            }

            dst_video_height = atoi(argv[3]);
        }
    }

    /* Create app model & UI */
    _model = new Model();
    _model->get_video_rec()->SetNewSource(dst_video_width, dst_video_height);
    _ui = new UI(app_name, app_version);

    /* Set default settings */
    _ui->get_video_fps_edit()->SetWndText(std::to_string(dst_video_fps).c_str());

    if (dst_video_width != -1)
    {
        _ui->get_video_width_edit()->SetWndText(std::to_string(dst_video_width).c_str());
    }
    if (dst_video_height != -1)
    {
        _ui->get_video_height_edit()->SetWndText(std::to_string(dst_video_height).c_str());
    }

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
    _ui->get_video_width_edit()->AddCallback("MainCallback", [this](void *ptr)->void {
        //Edit *edit = GetControl(Edit, ptr);

        _ui->get_video_settings_apply_button()->SetInputState(true);
        });

    _ui->get_video_height_edit()->AddCallback("MainCallback", [this](void *ptr)->void {
        //Edit *edit = GetControl(Edit, ptr);

        _ui->get_video_settings_apply_button()->SetInputState(true);
        });

    /* Buttons' callbacks */
    _ui->get_video_settings_apply_button()->AddCallback("MainCallback", [this, argv](void *ptr)->void {
        Button *button = GetControl(Button, ptr);

        /* Stop recording */
        _ui->get_stop_recording_menu_point()->operator()("MainCallback", _ui->get_stop_recording_menu_point());

        try
        {
            /* Set fps and resolution */
            char video_fps[6] = { 0 };
            char video_width[6] = { 0 };
            char video_height[6] = { 0 };

            _ui->get_video_fps_edit()->GetWndText(video_fps, 5);

            _ui->get_video_width_edit()->GetWndText(video_width, 5);
            _ui->get_video_height_edit()->GetWndText(video_height, 5);

            /* Fps */
            if (video_fps[0] == '\0')
            {
                throw std::string("FPS is not set!");
            }
            else if (atoi(video_fps) == 0 && video_fps[0] != '0')
            {
                throw std::string("Video FPS must be a number!");
            }

            /* Resolution */
            if (atoi(video_width) == 0 && video_width[0] != '0' && video_width[0] != '\0')
            {
                throw std::string("Video width must be a number!");
            }
            else if (atoi(video_height) == 0 && video_height[0] != '0' && video_height[0] != '\0')
            {
                throw std::string("Video height must be a number!");
            }

            /* Start new instance */
            STARTUPINFOA si = { 0 };
            PROCESS_INFORMATION pi = { 0 };
            si.cb = sizeof(si);
            //std::string test = std::string('\"' + std::string(argv[0]) + '\"' + std::string(" ") + (video_width[0] == '\0' ? "-1" : video_width) + " " + (video_height[0] == '\0' ? "-1" : video_height)).c_str();
            //std::cout << test << '\n';
            if (!CreateProcessA(argv[0], (LPSTR)std::string('\"' + std::string(argv[0]) + '\"' + std::string(" ") + (video_fps) + " " + (video_width[0] == '\0' ? "-1" : video_width) + " " + (video_height[0] == '\0' ? "-1" : video_height)).c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
            {
                throw std::string("Failed to create a new process!");
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            DestroyWindow(_ui->get_wnd()->GetHwnd());

            // Wait until child process exits.
            //WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
        catch (std::string error)
        {
            int str_size = (int)error.length() + 1;
            SmtObj<wchar_t[]> w_error = new wchar_t[str_size] { 0 };
            MultiByteToWideChar(CP_UTF8, 0, error.c_str(), str_size, w_error, str_size);

            MessageBoxW(NULL, w_error, L"Error", MB_OK);

            return;
        }

        button->SetInputState(false);
        });


    /* Menu points' callbacks */
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
