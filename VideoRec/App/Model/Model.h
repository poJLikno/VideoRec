#ifndef MODEL_H_
#define MODEL_H_

#include "VideoRecorder/VideoRecorder.h"
#include "FileNameGenerator/FileNameGenerator.h"

#ifndef GETTER
#define GETTER(obj) decltype(obj) &get##obj() { return obj; }
#endif

class Model
{
private:
    SmtObj<FileNameGenerator> _file_name_generator;

    SmtObj<VideoRecorder> _video_rec;
    bool _allow_preview_flag = true;
    bool _capture_client_rect_only_flag = true;
    bool _capture_optimization_flag = true;
    bool _capture_cursor_flag = false;

public:
    Model();
    Model(const Model &) = delete;
    ~Model() = default;

    GETTER(_file_name_generator)
    GETTER(_video_rec)
    GETTER(_allow_preview_flag)
    GETTER(_capture_client_rect_only_flag)
    GETTER(_capture_optimization_flag)
    GETTER(_capture_cursor_flag)
};

#endif /* MODEL_H_ */
