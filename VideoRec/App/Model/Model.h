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

public:
    Model();
    Model(const Model &) = delete;
    ~Model() = default;

    GETTER(_file_name_generator)
    GETTER(_video_rec)
};

#endif /* MODEL_H_ */
