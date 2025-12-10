#include "Utils/FileDialogManager.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <shobjidl_core.h>

#include "Utils/StringUtils.h"

FileDialogManager& FileDialogManager::Get()
{
    static FileDialogManager instance;
    return instance;
}

const wchar_t* FileDialogManager::GetFilterStringByType(DialogType type)
{
    switch (type)
    {
    case DialogType::Model:
        return FileCategories::Get()[0].filter; // 3D Models
    case DialogType::Texture:
        return FileCategories::Get()[1].filter; // Textures
    case DialogType::Audio:
        return FileCategories::Get()[2].filter; // Audio
    case DialogType::Font:
        return FileCategories::Get()[3].filter; // Fonts
    case DialogType::Script:
        return FileCategories::Get()[4].filter; // Scripts
    case DialogType::Shader:
        return FileCategories::Get()[5].filter; // Shaders
    case DialogType::All:
    default:
        return FileCategories::GetAllFilter(); // Alll files
    }
}

FileDialogManager::DialogType FileDialogManager::GetDialogTypeByExtension(const eastl::string& extension)
{
    eastl::string ext = extension;
    std::transform(ext.begin(), ext.end(), ext.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    if (!ext.empty() && ext[0] == '.')
        ext = ext.substr(1);
    
    const auto& categories = FileCategories::Get();
    
    for (size_t i = 0; i < categories.size(); ++i)
    {
        for (const auto& pattern : categories[i].patterns)
        {
            eastl::string patternExt = pattern.c_str();
            if (!patternExt.empty() && patternExt[0] == '.')
                patternExt = patternExt.substr(1);
            
            if (ext == patternExt)
            {
                switch (i)
                {
                case 0: return DialogType::Model;
                case 1: return DialogType::Texture;
                case 2: return DialogType::Audio;
                case 3: return DialogType::Font;
                case 4: return DialogType::Script;
                case 5: return DialogType::Shader;
                default: return DialogType::All;
                }
            }
        }
    }
    
    return DialogType::All;
}

std::filesystem::path FileDialogManager::OpenFile(
    DialogType type,
    const wchar_t* title,
    const std::filesystem::path& initialDir)
{
    wchar_t originalDir[MAX_PATH] = { 0 };
    GetCurrentDirectoryW(MAX_PATH, originalDir);
    
    OPENFILENAMEW ofn = {};
    wchar_t szFile[MAX_PATH] = { 0 };
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = GetFilterStringByType(type);
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = initialDir.empty() ? nullptr : initialDir.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    
    ofn.lpstrTitle = title;
    
    SetCurrentDirectoryW(originalDir);

    if (GetOpenFileNameW(&ofn))
        return std::filesystem::path(szFile);
    
    return {};
}

std::filesystem::path FileDialogManager::SaveFile(
    DialogType type,
    const wchar_t* title,
    const std::filesystem::path& initialDir,
    const wchar_t* defaultName)
{
    wchar_t originalDir[MAX_PATH] = { 0 };
    GetCurrentDirectoryW(MAX_PATH, originalDir);
    
    OPENFILENAMEW ofn;
    wchar_t szFile[MAX_PATH] = { 0 };
    
    if (defaultName)
        wcscpy_s(szFile, defaultName);
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = GetFilterStringByType(type);
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = initialDir.empty() ? nullptr : initialDir.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    
    ofn.lpstrTitle = title;

    SetCurrentDirectoryW(originalDir);
    
    if (GetSaveFileNameW(&ofn))
        return std::filesystem::path(szFile);
    
    return {};
}

std::filesystem::path FileDialogManager::SelectFolder(
    const wchar_t* title,
    const std::filesystem::path& initialDir)
{
    std::filesystem::path originalDir = std::filesystem::current_path();
    
    IFileDialog* pFileDialog;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, 
                                IID_IFileDialog, reinterpret_cast<void**>(&pFileDialog))))
        return {};
    
    DWORD dwOptions;
    pFileDialog->GetOptions(&dwOptions);
    pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);
    
    if (title)
        pFileDialog->SetTitle(title);
    
    if (!initialDir.empty())
    {
        IShellItem* pShellItem;
        if (SUCCEEDED(SHCreateItemFromParsingName(initialDir.c_str(), nullptr, 
                                                   IID_IShellItem, reinterpret_cast<void**>(&pShellItem))))
        {
            pFileDialog->SetFolder(pShellItem);
            pShellItem->Release();
        }
    }
    
    std::filesystem::path result;
    if (SUCCEEDED(pFileDialog->Show(nullptr)))
    {
        IShellItem* pShellResult;
        if (SUCCEEDED(pFileDialog->GetResult(&pShellResult)))
        {
            wchar_t* pszPath = nullptr;
            if (SUCCEEDED(pShellResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath)))
            {
                result = pszPath;
                CoTaskMemFree(pszPath);
            }
            pShellResult->Release();
        }
    }
    
    pFileDialog->Release();

    std::filesystem::current_path(originalDir);
    
    return result;
}