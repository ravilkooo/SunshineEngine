#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <optional>
#include "Utils/FileCategories.h"

class ContentBrowserPanel
{
public:
    ContentBrowserPanel();

    void OnImGuiRender();

    std::filesystem::path m_CurrentDirectory;
    static std::filesystem::path s_AssetsDirectory;

private:

    // Selection
    std::optional<std::filesystem::path> m_SelectedPath; 

    // Search
    char m_SearchBuffer[256] = {0};

    // Rename
    bool m_ShowRenamePopup = false;
    char m_RenameBuffer[260] = {0};

    // Delete confirm
    bool m_ShowDeleteConfirm = false;

    // Internal clipboard for copy/paste
    struct Clipboard {
        std::filesystem::path path;
        bool isCut = false; // cut vs copy
        bool hasValue = false;
    } m_Clipboard;
    
    // Helpers
    void DrawToolbar();
    void DrawItems();
    
    void ShowContextMenuFor(const std::filesystem::path& path);
    bool CopyPathToClipboardSystem(const std::string& text);
    
    bool FileCopy(const std::filesystem::path& src, const std::filesystem::path& dst);
    bool FileMove(const std::filesystem::path& src, const std::filesystem::path& dst);
    bool FileDelete(const std::filesystem::path& p);
    
    std::filesystem::path MakeUniquePath(const std::filesystem::path& dst);
    
    std::filesystem::path OpenFileDialog();
    std::filesystem::path SaveFileDialog(const wchar_t* filter, int filterIndex);
    
    std::pair<const wchar_t* , int> BuildFilterForType(const std::string& extension);

    // UI actions
    void Action_CreateFolder();
    void Action_Rename(const std::filesystem::path& p);
    void Action_Delete(const std::filesystem::path& p);
    void Action_Copy(const std::filesystem::path& p, bool cut);
    void Action_PasteToCurrent();
    void Action_Duplicate(const std::filesystem::path& p);
    void Action_Import();
    void Action_Export(const std::filesystem::path& p);
};

