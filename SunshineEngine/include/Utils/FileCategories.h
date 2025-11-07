#pragma once
#include <string>
#include <vector>

struct FileCategory
{
    std::string name;
    std::vector<std::string> patterns;
    const char* filter;
};

class FileCategories
{
public:
    static const std::vector<FileCategory>& Get();
    static const char* GetAllFilter();
};
