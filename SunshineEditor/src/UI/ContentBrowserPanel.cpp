#include "UI/ContentBrowserPanel.h"
#include <imgui.h>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <sstream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <commdlg.h>
#include <shlobj.h>

#include <Utils/StringUtils.h>
#include "Utils/FileDialogManager.h"

std::filesystem::path ContentBrowserPanel::s_AssetsDirectory =
    std::filesystem::path(JoinWchar_Wstring(PROJECTS_DIR, L"DefaultScene/").c_str());

ContentBrowserPanel::ContentBrowserPanel()
    : m_CurrentDirectory(s_AssetsDirectory)
{}

inline std::wstring UTF8ToWString(const std::string& str)
{
    if (str.empty()) return {};
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), wstr.data(), size_needed);
    return wstr;
}

inline std::string WStringToUTF8(const std::wstring& wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, str.data(), size_needed, nullptr, nullptr);
    return str;
}

bool ContentBrowserPanel::FileCopy(const std::filesystem::path& src, const std::filesystem::path& dst)
{
    if (std::filesystem::is_directory(dst)) {
        std::filesystem::path target = dst / src.filename();
        std::filesystem::copy_file(src, target, std::filesystem::copy_options::skip_existing);
    } else {
        std::filesystem::copy_file(src, dst, std::filesystem::copy_options::skip_existing);
    }
    return true;
}   

bool ContentBrowserPanel::FileMove(const std::filesystem::path& src, const std::filesystem::path& dst)
{
    std::filesystem::rename(src, dst);
    return true;
}

bool ContentBrowserPanel::FileDelete(const std::filesystem::path& p)
{
    if (std::filesystem::is_directory(p))
        std::filesystem::remove_all(p);
    else
        std::filesystem::remove(p);
    return true;
}

std::filesystem::path ContentBrowserPanel::MakeUniquePath(const std::filesystem::path& dst)
{
    if (!std::filesystem::exists(dst)) return dst;

    auto parent = dst.parent_path();
    auto stem = dst.stem().string();
    auto ext = dst.extension().string();

    int counter = 1;
    while (true) {
        std::ostringstream oss;
        oss << stem << " (" << counter << ")" << ext;
        std::filesystem::path candidate = parent / oss.str();
        if (!std::filesystem::exists(candidate)) return candidate;
        ++counter;
    }
}

bool ContentBrowserPanel::CopyPathToClipboardSystem(const std::string& text)
{
    if (!OpenClipboard(NULL)) return false;
    EmptyClipboard();
    
    size_t sizeInBytes = (text.size() + 1) * sizeof(char);
    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, sizeInBytes);
    if (!hGlob)
    {
        CloseClipboard();
        return false;
    }
    
    void* pData = GlobalLock(hGlob);
    memcpy(pData, text.c_str(), sizeInBytes);
    GlobalUnlock(hGlob);
    SetClipboardData(CF_TEXT, hGlob);
    CloseClipboard();
    return true;
}

std::filesystem::path ContentBrowserPanel::OpenFileDialog()
{
    // OPENFILENAMEW ofn;       
    // wchar_t szFile[260] = { 0 };
    // ZeroMemory(&ofn, sizeof(ofn));
    // ofn.lStructSize = sizeof(ofn);
    // ofn.hwndOwner = NULL;
    // ofn.lpstrFile = szFile;
    // ofn.nMaxFile = _countof(szFile);
    // ofn.lpstrFilter = FileCategories::GetAllFilter();
    // ofn.nFilterIndex = 1;
    // ofn.lpstrFileTitle = NULL;
    // ofn.nMaxFileTitle = 0;
    // ofn.lpstrInitialDir = NULL;
    // ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    //
    // if (GetOpenFileNameW(&ofn))
    //     return std::filesystem::u8path(WStringToUTF8(szFile));
    // return {};

    return FileDialogManager::Get().OpenFile(
         FileDialogManager::DialogType::All,
         L"Open File"
     );
}

std::filesystem::path ContentBrowserPanel::SaveFileDialog(const wchar_t* filter, int filterIndex)
{
    OPENFILENAMEW ofn;
    wchar_t szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = _countof(szFile);
    ofn.lpstrFilter = filter;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    ofn.nFilterIndex = filterIndex;

    if (GetSaveFileNameW(&ofn))
        return std::filesystem::u8path(WStringToUTF8(szFile));

    return {};
}

std::pair<const wchar_t* , int> ContentBrowserPanel::BuildFilterForType(const std::string& extension)
{
    std::string lowerExt = extension;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);

    const auto& categories = FileCategories::Get();

    for (const auto& cat : categories)
    {
        auto it = std::find(cat.patterns.begin(), cat.patterns.end(), lowerExt);
        if (it != cat.patterns.end())
        {
            int index = static_cast<int>(std::distance(cat.patterns.begin(), it)) + 2;
            return { cat.filter, index };
        }
    }

    return { L"All Files (*.*)\0*.*\0\0", 1 };
}


// ---------------- UI Actions ----------------

void ContentBrowserPanel::Action_CreateFolder()
{
    // Create "NewFolder" or "NewFolder (n)"
    std::filesystem::path newFolder = m_CurrentDirectory / "New Folder";
    int counter = 1;
    while (std::filesystem::exists(newFolder)) {
        std::ostringstream oss;
        oss << "New Folder (" << counter << ")";
        newFolder = m_CurrentDirectory / oss.str();
        ++counter;
    }
    std::filesystem::create_directory(newFolder);
}

void ContentBrowserPanel::Action_Rename(const std::filesystem::path& p)
{
    // Open rename popup and prefill buffer with filename
    m_ShowRenamePopup = true;
    std::string filename = WStringToUTF8(p.filename().wstring());
    //std::string filename = p.filename().u8string();
    
    strncpy(m_RenameBuffer, filename.c_str(), sizeof(m_RenameBuffer)-1);
    m_RenameBuffer[sizeof(m_RenameBuffer)-1] = '\0';
    
    m_SelectedPath = p;
}

void ContentBrowserPanel::Action_Delete(const std::filesystem::path& p)
{
    m_ShowDeleteConfirm = true;
    m_SelectedPath = p;
}

void ContentBrowserPanel::Action_Copy(const std::filesystem::path& p, bool cut)
{
    m_Clipboard.path = p;
    m_Clipboard.isCut = cut;
    m_Clipboard.hasValue = true;
}

void ContentBrowserPanel::Action_PasteToCurrent()
{
    if (!m_Clipboard.hasValue) return;
    std::filesystem::path src = m_Clipboard.path;
    std::filesystem::path dst = m_CurrentDirectory / src.filename();
    dst = MakeUniquePath(dst);

    if (m_Clipboard.isCut) {
        FileMove(src, dst);
    } else {
            if (std::filesystem::is_directory(src)) {
                std::filesystem::copy(src, dst, std::filesystem::copy_options::recursive | std::filesystem::copy_options::skip_existing);
            } else {
                std::filesystem::copy_file(src, dst, std::filesystem::copy_options::skip_existing);
            }
    }
    
    if (m_Clipboard.isCut)
    {
        m_Clipboard.hasValue = false; // Clear after move
    }
}

void ContentBrowserPanel::Action_Duplicate(const std::filesystem::path& p)
{
    std::filesystem::path dst = p.parent_path() / p.filename();
    dst = MakeUniquePath(dst);
    if (std::filesystem::is_directory(p)) {
        std::filesystem::copy(p, dst, std::filesystem::copy_options::recursive);
    } else {
        std::filesystem::copy_file(p, dst);
    }

}

void ContentBrowserPanel::Action_Import()
{
    // auto selectedFile = OpenFileDialog();
    // if (!selectedFile.empty())
    // {
    //     std::filesystem::path dst = m_CurrentDirectory / selectedFile.filename();
    //     
    //     dst = MakeUniquePath(dst);
    //     FileCopy(selectedFile, dst);
    // }
    
    auto selectedFile = FileDialogManager::Get().OpenFile(
        FileDialogManager::DialogType::All,
        L"Import File"
    );
    
    if (!selectedFile.empty())
    {
        std::filesystem::path dst = m_CurrentDirectory / selectedFile.filename();
        dst = MakeUniquePath(dst);
        FileCopy(selectedFile, dst);
    }
}

void ContentBrowserPanel::Action_Export(const std::filesystem::path& p)
{
    std::string ext = p.extension().string();
    
    auto dialogType = FileDialogManager::GetDialogTypeByExtension(StdToEASTLString(ext));
    
    auto dstPath = FileDialogManager::Get().SaveFile(
        dialogType,
        L"Export File",
        {},
        p.filename().c_str()
    );
    
    if (!dstPath.empty())
    {
        if (std::filesystem::path(dstPath).extension().empty())
            dstPath += ext;
    
        std::filesystem::path target = dstPath;
        if (std::filesystem::is_directory(dstPath))
            target = dstPath / p.filename();
        target = MakeUniquePath(target);
        FileCopy(p, target);
    }
}

void ContentBrowserPanel::DrawToolbar()
{
    ImGui::SameLine();
    if (ImGui::Button("Import"))
    {
        Action_Import();
    }
    ImGui::SameLine();
    if (ImGui::Button("New Folder"))
    {
        Action_CreateFolder();
    }
    ImGui::SameLine();
    if (ImGui::Button("Paste"))
    {
        Action_PasteToCurrent();
    }
    ImGui::SameLine();
    ImGui::InputTextWithHint("##search", "Search...", m_SearchBuffer, sizeof(m_SearchBuffer));


}

void ContentBrowserPanel::ShowContextMenuFor(const std::filesystem::path& path)
{
    // if (ImGui::BeginPopupContextItem(("context_" + path.string()).c_str()))
    if (ImGui::BeginPopupContextItem(("context_" + WStringToUTF8(path.filename().wstring())).c_str()))
    {
        if (ImGui::MenuItem("Copy")) { Action_Copy(path, false); }
        if (ImGui::MenuItem("Cut")) { Action_Copy(path, true); }
        if (ImGui::MenuItem("Paste", nullptr, false, m_Clipboard.hasValue)) { Action_PasteToCurrent(); }
        if (ImGui::MenuItem("Duplicate")) { Action_Duplicate(path); }
        if (ImGui::MenuItem("Rename")) { Action_Rename(path); }
        if (ImGui::MenuItem("Delete")) { Action_Delete(path); }
        if (ImGui::MenuItem("Copy Path")) { CopyPathToClipboardSystem(path.string()); }
        if (ImGui::MenuItem("Export...")) { Action_Export(path); }
        ImGui::EndPopup();
    }
}

void ContentBrowserPanel::DrawItems()
{
    // Walk directory and show items; apply search filter
    std::vector<std::filesystem::directory_entry> entries;
    for (auto& e : std::filesystem::directory_iterator(m_CurrentDirectory))
        entries.push_back(e);

    // Sort: folders first then files
    std::sort(entries.begin(), entries.end(), [](auto &a, auto &b) {
        if (a.is_directory() != b.is_directory()) return a.is_directory();
        return a.path().filename().string() < b.path().filename().string();
    });

    // Iterate
    ImGui::BeginChild("##content_browser_items", ImVec2(0, 0), false);
    for (auto& directoryEntry : entries)
    {
        auto& path = directoryEntry.path();
        // auto filename = path.filename().string();
        auto filename = WStringToUTF8(path.filename().wstring());

        // Search filter
        std::string filter(m_SearchBuffer);
        if (!filter.empty()) {
            std::string lowerName = filename;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            std::transform(filter.begin(), filter.end(), filter.begin(), ::tolower);
            if (lowerName.find(filter) == std::string::npos) continue;
        }

        // Show icon/text as button
        ImGui::PushID(filename.c_str());
        ImGui::BeginGroup();
        if (directoryEntry.is_directory())
        {
            // Folder icon (text) and clickable
            if (ImGui::Button((std::string("[DIR] ") + filename).c_str()))
            {
                m_CurrentDirectory /= path.filename();
            }
        }
        else
        {
            if (ImGui::Button(filename.c_str()))
            {
                // Select file
                m_SelectedPath = path;
            }
        }

        // Right-click context menu for the item
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup(("context_" + path.string()).c_str());
        }
        ShowContextMenuFor(path);

        ImGui::SameLine();
        // Show small metadata: size 
        if (!directoryEntry.is_directory())
        {
            auto fsize = std::filesystem::file_size(path);
            ImGui::Text("(%llu bytes)", (unsigned long long)fsize);
        }

        ImGui::EndGroup();
        ImGui::PopID();
    }
    ImGui::EndChild();
}

void ContentBrowserPanel::OnImGuiRender()
{
    ImGui::Begin("Content Browser");

    // Up button
    if (m_CurrentDirectory != s_AssetsDirectory)
    {
        if (ImGui::Button("<-"))
        {
            m_CurrentDirectory = m_CurrentDirectory.parent_path();
        }
        ImGui::SameLine();
    }
    // ImGui::Text("%s", std::filesystem::relative(m_CurrentDirectory, s_AssetsDirectory.parent_path()).string().c_str());
    ImGui::Text("%s", WStringToUTF8(std::filesystem::relative(m_CurrentDirectory, s_AssetsDirectory.parent_path()).wstring()).c_str());

    // Toolbar
    DrawToolbar();

    // Items
    DrawItems();

    // ---- Rename popup ----
    if (m_ShowRenamePopup && m_SelectedPath.has_value())
    {
        ImGui::OpenPopup("Rename");
        if (ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Rename:");
            ImGui::InputText("##rename", m_RenameBuffer, sizeof(m_RenameBuffer));
            if (ImGui::Button("OK"))
            {
                // std::string newName(m_RenameBuffer);
                auto oldPath = *m_SelectedPath;
                // auto newPath = oldPath.parent_path() / std::filesystem::u8path(m_RenameBuffer);
                auto newPath = oldPath.parent_path() / UTF8ToWString(m_RenameBuffer);
                newPath = MakeUniquePath(newPath);
                FileMove(oldPath, newPath);
                
                m_ShowRenamePopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                m_ShowRenamePopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    // ---- Delete confirm popup ----
    if (m_ShowDeleteConfirm && m_SelectedPath.has_value())
    {
        ImGui::OpenPopup("Delete?");
        if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            // ImGui::Text("Delete %s ?", m_SelectedPath->filename().string().c_str());
            ImGui::Text("Delete %s ?", WStringToUTF8(m_SelectedPath->filename().wstring()).c_str());

            if (ImGui::Button("Yes"))
            {
                FileDelete(*m_SelectedPath);
                m_ShowDeleteConfirm = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("No"))
            {
                m_ShowDeleteConfirm = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    
    ImGui::End();
}

