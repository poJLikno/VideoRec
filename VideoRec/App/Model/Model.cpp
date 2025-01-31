#include "Model.h"

Model::Model()
{
    _video_rec = new VideoRecorder();
    _file_name_generator = new FileNameGenerator();
}
