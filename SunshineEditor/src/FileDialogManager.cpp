#include "FileDialogManager.h"
#include <sstream>
#include <algorithm>
#include <shobjidl_core.h>

FileDialogManager& FileDialogManager::Get()
{
    static FileDialogManager instance;
    return instance;
}

std::wstring FileDialogManager::ToWString(const std::string& utf8)
{
    if (utf8.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), wstr.data(), size_needed);
    return wstr;
}

std::string FileDialogManager::ToUTF8(const std::wstring& wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, str.data(), size_needed, nullptr, nullptr);
    return str;
}

std::wstring FileDialogManager::CreateFilterString(const std::vector<FileFilter>& filters)
{
    std::wstringstream ss;
    for (const auto& filter : filters)
    {
        ss << filter.description << L'\0' << filter.pattern << L'\0';
    }
    ss << L'\0'; 
    return ss.str();
}

std::filesystem::path FileDialogManager::OpenFile(
    const std::wstring& title,
    const std::vector<FileFilter>& filters,
    const std::filesystem::path& initialDir)
{
    OPENFILENAMEW ofn;
    wchar_t szFile[MAX_PATH] = { 0 };
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = CreateFilterString(filters).c_str();
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = initialDir.empty() ? nullptr : initialDir.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    
    if (title.empty())
        ofn.lpstrTitle = nullptr;
    else
        ofn.lpstrTitle = title.c_str();
    
    if (GetOpenFileNameW(&ofn))
        return std::filesystem::path(szFile);
    
    return {};
}

std::filesystem::path FileDialogManager::SaveFile(
    const std::wstring& title,
    const std::vector<FileFilter>& filters,
    const std::filesystem::path& initialDir,
    const std::wstring& defaultName)
{
    OPENFILENAMEW ofn;
    wchar_t szFile[MAX_PATH] = { 0 };
    
    if (!defaultName.empty())
        wcscpy_s(szFile, defaultName.c_str());
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = CreateFilterString(filters).c_str();
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = initialDir.empty() ? nullptr : initialDir.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    
    if (title.empty())
        ofn.lpstrTitle = nullptr;
    else
        ofn.lpstrTitle = title.c_str();
    
    if (GetSaveFileNameW(&ofn))
        return std::filesystem::path(szFile);
    
    return {};
}

std::filesystem::path FileDialogManager::SelectFolder(
    const std::wstring& title,
    const std::filesystem::path& initialDir)
{
    IFileDialog* pFileDialog;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, 
                                IID_IFileDialog, reinterpret_cast<void**>(&pFileDialog))))
        return {};
    
    DWORD dwOptions;
    pFileDialog->GetOptions(&dwOptions);
    pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);
    
    if (!title.empty())
        pFileDialog->SetTitle(title.c_str());
    
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
    return result;
}

std::vector<FileDialogManager::FileFilter> FileDialogManager::Get3DModelFilters()
{
    return {
        { L"3D Models", L"*.fbx;*.obj;*.dae;*.blend;*.3ds;*.max;*.x" },
        { L"FBX Files", L"*.fbx" },
        { L"OBJ Files", L"*.obj" },
        { L"Collada Files", L"*.dae" },
        { L"All Files", L"*.*" }
    };
}

std::vector<FileDialogManager::FileFilter> FileDialogManager::GetImageFilters()
{
    return {
        { L"Image Files", L"*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.dds" },
        { L"PNG Files", L"*.png" },
        { L"JPEG Files", L"*.jpg;*.jpeg" },
        { L"All Files", L"*.*" }
    };
}

std::vector<FileDialogManager::FileFilter> FileDialogManager::GetAllFilters()
{
    return { { L"All Files", L"*.*" } };
}

std::vector<FileDialogManager::FileFilter> FileDialogManager::GetSpecificFilter(
    const std::wstring& description, const std::wstring& pattern)
{
    return { { description, pattern } };
}