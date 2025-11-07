#pragma once
#include <string>
#include <vector>

struct FileCategory
{
    std::string name;
    std::vector<std::string> patterns;
    const wchar_t* filter;
};

class FileCategories
{
public:
    static const std::vector<FileCategory>& Get();
    static const wchar_t* GetAllFilter();
};
