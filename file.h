#ifndef FILE_H
#define FILE_H

#include <filesystem>
#include <string>
#include <string_view>
#include <fstream>

namespace file
{

namespace fs = std::filesystem;

static std::string absoute_file_path(std::string_view filename);

enum class Write_Mode
{
    OverWrite,
    Append
};

class File
{
public:
    File(std::string_view filename = "");
    bool is_exists() const;
    bool create() const;
    void set_file_name(std::string_view filename);
    bool write(std::string_view content, Write_Mode mode, bool auto_create = true);
    void read(std::string &buffer);
private:
    std::string filename;
};

}
#endif
