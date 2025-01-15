#include "Model.h"

Model::Model()
{
    _file_name_generator = new FileNameGenerator();
}

void Model::ResetAll()
{
    _video_rec.reset();
    //_dshow_rec.reset();
}
