#include "Model.h"

Model::Model()
{
    _video_rec = std::unique_ptr<VideoRecorder>(new VideoRecorder(_capture_client_rect_only_flag, _capture_optimization_flag, _capture_cursor_flag));
    _file_name_generator = std::make_unique<FileNameGenerator>();
}
