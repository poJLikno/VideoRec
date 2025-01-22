#include "Model.h"

Model::Model()
{
    _file_name_generator = new FileNameGenerator();
}

void Model::ResetAllVideo()
{
    _video_rec.reset();
    _dshow_rec->StopRecording();
    _dshow_rec->GetVideoDevice()->ReleaseDevice();
    _dshow_rec->StopRecording();/* Need to reset DShow preview window */
}
