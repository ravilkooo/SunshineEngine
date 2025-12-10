#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <Windows.h>

class FileDialogManager
{
public:
    struct FileFilter
    {
        std::wstring description;
        std::wstring pattern;
    };
    
    static FileDialogManager& Get();
    
    std::filesystem::path OpenFile(
        const std::wstring& title = L"Open File",
        const std::vector<FileFilter>& filters = GetAllFilters(),
        const std::filesystem::path& initialDir = {}
    );
    
    std::filesystem::path SaveFile(
        const std::wstring& title = L"Save File",
        const std::vector<FileFilter>& filters = GetAllFilters(),
        const std::filesystem::path& initialDir = {},
        const std::wstring& defaultName = {}
    );
    
    std::filesystem::path SelectFolder(
        const std::wstring& title = L"Select Folder",
        const std::filesystem::path& initialDir = {}
    );
    
    static std::vector<FileFilter> Get3DModelFilters();
    static std::vector<FileFilter> GetImageFilters();
    static std::vector<FileFilter> GetAllFilters();
    static std::vector<FileFilter> GetSpecificFilter(const std::wstring& description, const std::wstring& pattern);
    
    static std::wstring ToWString(const std::string& utf8);
    static std::string ToUTF8(const std::wstring& wstr);
    
private:
    FileDialogManager() = default;
    ~FileDialogManager() = default;
    
    std::wstring CreateFilterString(const std::vector<FileFilter>& filters);
};