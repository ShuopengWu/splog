#include "file.h"

file::File::File(std::string_view filename)
    : filename(absoute_file_path(filename))
{
}

bool file::File::is_exists() const
{
    if (filename.empty())
        return false;

    fs::path filepath(filename);
    return fs::exists(filepath) && fs::is_regular_file(filepath);
}

bool file::File::create() const
{
    if (filename.empty())
        return false;

    if (is_exists())
        return true;

    fs::path filepath(filename);
    std::error_code error_code;

    fs::path parent_path = filepath.parent_path();
    if (!parent_path.empty() && !fs::exists(parent_path))
    {
        if (!fs::create_directories(parent_path, error_code))
            return false;

        if (error_code)
            return false;
    }

    std::ofstream ofs(filename, std::ios::out);
    if (ofs.is_open())
    {
        ofs.close();
        return true;
    }

    return false;
}

void file::File::set_file_name(std::string_view filename)
{
    if (filename.empty())
        return;

    this->filename = absoute_file_path(filename);
}

bool file::File::write(std::string_view content, Write_Mode mode, bool auto_create)
{
    if (filename.empty() || content.empty())
        return false;

    if (!is_exists())
    {
        if (!auto_create)
            return false;

        if (!create())
            return false;
    }

    std::ios::openmode open_flags = std::ios::out | std::ios::binary;

    switch (mode)
    {
    case Write_Mode::Append:    open_flags |= std::ios::app;    break;
    case Write_Mode::OverWrite: open_flags |= std::ios::trunc;  break;
    default:    break;
    }

    std::ofstream ofs(filename, open_flags);

    if (ofs && ofs.is_open())
    {
        ofs.write(content.data(), content.size());

        bool success = ofs.good();
        ofs.close();

        if (ofs.fail())
            return false;

        return success;
    }

    return false;
}

void file::File::read(std::string &buffer)
{
    if (!buffer.empty())
        buffer.clear();

    if (filename.empty() || !is_exists())
        return;

    std::error_code ec;
    uintmax_t size = fs::file_size(filename, ec);
    if (ec || size == 0)
        return;


    std::ifstream ifs(filename, std::ios::in | std::ios::binary);

    if (ifs && ifs.is_open())
    {
        if (buffer.size() < size)
            buffer.resize(size);

        ifs.read(buffer.data(), size);

        if(!ifs.good() && !ifs.fail())
            buffer.clear();

        ifs.close();
    }
}

std::string file::absoute_file_path(std::string_view filename)
{
    if (filename.empty())
        return "";

    fs::path filepath(filename);
    if (filepath.is_absolute())
        return filepath.string();

    filepath = fs::current_path() / filename;

    std::error_code ec;
    filepath = fs::absolute(filepath, ec);

    if (ec)
    {
        std::string error_message = ec.message();
        return "";
    }
    return filepath.string();
}
