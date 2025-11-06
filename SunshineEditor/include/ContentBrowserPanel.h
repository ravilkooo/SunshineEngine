#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <optional>


class ContentBrowserPanel
{
public:
    ContentBrowserPanel();

    void OnImGuiRender();

private:
    std::filesystem::path m_CurrentDirectory;
    static std::filesystem::path s_AssetsDirectory;

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


private:
    // Helpers
    void DrawToolbar();
    void DrawItems();
    void ShowContextMenuFor(const std::filesystem::path& path);
    bool CopyPathToClipboardSystem(const std::string& text); 
    bool FileCopy(const std::filesystem::path& src, const std::filesystem::path& dst);
    bool FileMove(const std::filesystem::path& src, const std::filesystem::path& dst);
    bool FileDelete(const std::filesystem::path& p);
    std::filesystem::path MakeUniquePath(const std::filesystem::path& dst);
    std::string OpenFileDialog();
    std::string SaveFileDialog();
    const char* filter =
        "All Supported Assets\0"
        "*.fbx;*.obj;*.abc;*.usd;"
        "*.jpg;*.jpeg;*.png;*.tga;*.bmp;*.psd;*.dds;*.tiff;*.gif;*.hdr;"
        "*.wav;*.mp3;*.ogg;"
        "*.ttf;*.otf;"
        "*.lua;"
        "*.hlsl;*.glsl;*.vert;*.frag;*.shader;\0"
        "3D Models (*.fbx;*.obj;*.abc;*.usd)\0*.fbx;*.obj;*.abc;*.usd\0"
        "Textures (*.jpg;*.jpeg;*.png;*.tga;*.bmp;*.psd;*.dds;*.tiff;*.gif;*.hdr)\0*.jpg;*.jpeg;*.png;*.tga;*.bmp;*.psd;*.dds;*.tiff;*.gif;*.hdr\0"
        "Audio (*.wav;*.mp3;*.ogg)\0*.wav;*.mp3;*.ogg\0"
        "Fonts (*.ttf;*.otf)\0*.ttf;*.otf\0"
        "Scripts (*.lua)\0*.lua\0"
        "Shaders (*.hlsl;*.glsl;*.vert;*.frag;*.shader)\0*.hlsl;*.glsl;*.vert;*.frag;*.shader\0"
        "\0"
        "All Files\0*.*\0";
    /*const char* filter =
    "Supported Assets (3D, Textures, Audio, Fonts, Scripts, Shaders)\0"
    "*.fbx;*.obj;*.abc;*.usd;"
    "*.jpg;*.jpeg;*.png;*.tga;*.bmp;*.psd;*.dds;*.tiff;*.gif;*.hdr;"
    "*.wav;*.mp3;*.ogg;"
    "*.ttf;*.otf;"
    "*.lua;"
    "*.hlsl;*.glsl;*.vert;*.frag;*.shader;"
    "\0"
    "All Files\0*.*\0";*/



    // UI actions
    void Action_CreateFolder();
    void Action_Rename(const std::filesystem::path& p);
    void Action_Delete(const std::filesystem::path& p);
    void Action_Copy(const std::filesystem::path& p, bool cut);
    void Action_PasteToCurrent();
    void Action_Duplicate(const std::filesystem::path& p);
    void Action_Import();
    void Action_Export();
};
