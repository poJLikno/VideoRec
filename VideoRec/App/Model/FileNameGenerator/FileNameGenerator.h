#ifndef FILE_NAME_GENERATOR_H_
#define FILE_NAME_GENERATOR_H_

#include "../../../SmtObj.h"

class FileNameGenerator
{
private:
    size_t _thread_id = 0;
    SmtObj<char[]> _user_name;
    SmtObj<char[]> _file_name;

public:
    FileNameGenerator();
    FileNameGenerator(const FileNameGenerator &) = delete;
    ~FileNameGenerator() = default;

    const SmtObj<char[]> &CreateFileName();
};

#endif /* FILE_NAME_GENERATOR_H_ */
