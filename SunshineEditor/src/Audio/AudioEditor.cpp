#include "Audio/AudioEditor.h"
#include <fstream>
#include <algorithm>
#include <set>
#include "Utils/FileCategories.h"
#include "Utils/FileSystemWrapper.h"
#include <EditorLogManager.h>
#include "EASTL/string.h"
#include "Utils/StringUtils.h"
#include <Utils/AssetPath.h>

eastl::wstring AudioEditor::s_configPath = []() {
    return JoinWchar_Wstring(AssetPath::s_projectPath.c_str(), L"Audio/audio_tracks.json");
        //JoinWchar_Wstring(ENGINE_ASSETS_DIR, L"Config/audio_tracks.json");
}();

std::vector<AssetPath> AudioEditor::m_audioFiles;

void AudioEditor::ScanAudioFiles()
{
    m_audioFiles.clear();
    std::error_code ec;

    AssetPath audioDirPath(L"Audio/", AssetPath::AssetSource::Project);
    AssetPath currentAudio(L"Audio/", AssetPath::AssetSource::Project);
    //for (eastlfs::directory_iterator it(dirPath); it != eastlfs::directory_iterator(""); ++it) {
    eastlfs::directory_iterator end;
    for (eastlfs::directory_iterator it(WStringToUtf8(audioDirPath.GetFullPath()), ec); it != end; ++it)
    {
        auto& entry = it.entry();
        if (eastlfs::is_regular_file(entry))
        {
            eastl::string filename = eastlfs::filename(entry);
            currentAudio.m_assetRelativePath = L"Audio/" + Utf8ToWString(filename);

            if (filename.size() > 4 && EASTLStringEqualsChar(filename.substr(filename.size() - 4), ".wav"))
                m_audioFiles.push_back(currentAudio);
        }
    }
    if (ec)
    {
        LOG_EDITOR_ERROR("Audio dir not found: %s\n", WStringToUtf8(audioDirPath.GetFullPath()).c_str());
    }

    /*
    std::error_code ec;
    if (!std::filesystem::exists(scriptPath.GetFullPath().c_str()))
    {
        wprintf(L"Lua file not found: %ls\n", scriptPath.GetFullPath().c_str());
    }
    */
}

void AudioEditor::Update() {
    if (m_previewSystem) {
        m_previewSystem->Update();
    }
}

void AudioEditor::SetConfigPath(const eastl::wstring& path)
{
    s_configPath = path;
}

const eastl::wstring& AudioEditor::GetConfigPath()
{
    return s_configPath;
}

void AudioEditor::AddTrack(const AudioTrack& track) {
    for (const auto& existing : m_trackList) {
        if (existing.name == track.name) {
            LOG_EDITOR_WARN("Audio track with name '%s' already exists!", track.name.c_str());
            return;
        }
    }

    m_trackList.push_back(track);
    SaveToJson(); 
}

void AudioEditor::RemoveTrack(std::string name) {
    auto it = std::remove_if(m_trackList.begin(), m_trackList.end(), 
        [&](const AudioTrack& t) { return t.name == name; });
    
    if (it != m_trackList.end()) {
        m_trackList.erase(it, m_trackList.end());
        SaveToJson();
    }
}

void AudioEditor::PlayPreview(std::string name) {
    if (!m_previewSystem) return;
    if (s_configPath.empty()) return;

    std::filesystem::path previewPath =
        std::filesystem::path(s_configPath.c_str()).parent_path()
        / "audio_preview.json";

    SaveToJson(Utf8ToWString(previewPath.string().c_str()));
    m_previewSystem->LoadFromJson(previewPath.string());
    m_previewSystem->Play(name);
}

void AudioEditor::StopPreview() {
    if (m_previewSystem) m_previewSystem->StopAll();
}

void AudioEditor::SetLoop(std::string name, bool loop) {
    if (auto* t = getTrack(name)) {
        t->loop = loop;
        SaveToJson();
    }
}

void AudioEditor::SetVolume(std::string name, float volume) {
    if (auto* t = getTrack(name)) {
        t->volume = volume;
        SaveToJson();
    }
}

AudioTrack* AudioEditor::getTrack(std::string name) {
    for (auto& t : m_trackList) {
        if (t.name == name) return &t;
    }
    return nullptr;
}

AudioTrack* AudioEditor::getTrack(AssetPath name) {
    for (auto& t : m_trackList) {
        if (t.filePath == name) return &t;
    }
    return nullptr;
}

void AudioEditor::SaveToJson()
{
    SaveToJson(s_configPath);
}

void AudioEditor::SaveToJson(const eastl::wstring& path)
{
    if (path.empty()) return;
    if (m_trackList.empty()) return;

    std::filesystem::create_directories(
        std::filesystem::path(path.c_str()).parent_path()
    );

    json j;
    j["tracks"] = json::array();
    for (const auto& track : m_trackList)
    {
        j["tracks"].push_back(track.ToJson());
    }

    std::ofstream file(path.c_str());
    if (!file.is_open()) return;

    file << j.dump(4);
}

bool AudioEditor::LoadFromJson()
{
    if (s_configPath.empty()) return false;

    std::ifstream file(s_configPath.c_str());
    if (!file.is_open()) {
        LOG_EDITOR_WARN("Audio config not found: %s", s_configPath.c_str());
        return false;
    }

    json j;
    try {
        file >> j;
    } catch (...) {
        LOG_EDITOR_ERROR("Invalid audio json: %s", s_configPath.c_str());
        return false;
    }

    if (!j.contains("tracks") || !j["tracks"].is_array())
        return false;

    m_trackList.clear();

    for (const auto& objJ : j["tracks"]) {
        AudioTrack track;
        track.FromJson(objJ);
        m_trackList.push_back(track);
    }

    return true;
}

void AudioEditor::RenameTrack(const std::string& oldName, const std::string& newName)
{
    for (auto& t : m_trackList)
    {
        if (t.name == oldName)
        {
            t.name = newName;
            break;
        }
    }

    SaveToJson();
}