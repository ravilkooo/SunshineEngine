#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <Windows.h>
#include "Utils/FileCategories.h"
#include <EASTL/string.h>

class FileDialogManager
{
public:
    enum class DialogType
    {
        Model,
        Texture,
        Audio,
        Font,
        Script,
        Shader,
        All
    };
    
    static FileDialogManager& Get();
    
    std::filesystem::path OpenFile(
        DialogType type = DialogType::All,
        const wchar_t* title = L"Open File",
        const std::filesystem::path& initialDir = {}
    );
    
    std::filesystem::path SaveFile(
        DialogType type = DialogType::All,
        const wchar_t* title = L"Save File",
        const std::filesystem::path& initialDir = {},
        const wchar_t* defaultName = nullptr
    );

    std::filesystem::path SelectFolder(
        const wchar_t* title = L"Select Folder",
        const std::filesystem::path& initialDir = {}
    );
    
    static DialogType GetDialogTypeByExtension(const eastl::string& extension);
    
private:
    FileDialogManager() = default;
    ~FileDialogManager() = default;
    
    static const wchar_t* GetFilterStringByType(DialogType type);
};