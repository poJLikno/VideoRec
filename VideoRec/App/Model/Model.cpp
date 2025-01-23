#include "Model.h"

Model::Model()
{
    _video_rec = new VideoRecorder(_allow_preview_flag);
    _file_name_generator = new FileNameGenerator();
}
