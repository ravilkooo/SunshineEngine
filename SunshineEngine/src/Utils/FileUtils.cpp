#include <Utils/FileUtils.h>
#include <system_error>

bool FileExistsNoThrow(const std::filesystem::path& p)
{
    std::error_code ec;
    return std::filesystem::exists(p, ec) && std::filesystem::is_regular_file(p, ec) && !ec;
}