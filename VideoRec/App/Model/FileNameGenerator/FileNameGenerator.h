#ifndef FILE_NAME_GENERATOR_H_
#define FILE_NAME_GENERATOR_H_

#include <memory>

class FileNameGenerator
{
private:
    size_t _thread_id = 0;
    std::unique_ptr<char[]> _user_name;
    std::unique_ptr<char[]> _file_name;

public:
    FileNameGenerator();
    FileNameGenerator(const FileNameGenerator &) = delete;
    ~FileNameGenerator() = default;

    const char *CreateFileName();
};

#endif /* FILE_NAME_GENERATOR_H_ */
