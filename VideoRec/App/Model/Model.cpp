#include "Model.h"

Model::Model()
{
    _video_rec = new VideoRecorder(_capture_client_rect_only_flag, _allow_preview_flag, _capture_optimization_flag);
    _file_name_generator = new FileNameGenerator();
}
