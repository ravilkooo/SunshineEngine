#include "Audio/AudioEditor.h"
#include <fstream>
#include <algorithm>
#include <set>
#include "Utils/FileCategories.h"
#include "LogManager.h"
#include "EASTL/string.h"
#include "Utils/StringUtils.h"

std::string AudioEditor::s_configPath = []() {
    eastl::wstring w =
        JoinWchar_Wstring(ENGINE_ASSETS_DIR, L"Config/audio_tracks.json");
    return std::string(w.begin(), w.end());
}();

void AudioEditor::Update() {
    if (m_previewSystem) {
        m_previewSystem->Update();
    }
}

const std::string& AudioEditor::GetConfigPath()
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
        std::filesystem::path(s_configPath).parent_path()
        / "audio_preview.json";

    SaveToJson(previewPath.string());
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

void AudioEditor::SaveToJson()
{
    SaveToJson(s_configPath);
}

void AudioEditor::SaveToJson(const std::string& path)
{
    if (path.empty()) return;
    if (m_trackList.empty()) return;

    std::filesystem::create_directories(
        std::filesystem::path(path).parent_path()
    );

    json j;
    j["tracks"] = m_trackList;

    std::ofstream file(path);
    if (!file.is_open()) return;

    file << j.dump(4);
}

bool AudioEditor::LoadFromJson()
{
    if (s_configPath.empty()) return false;

    std::ifstream file(s_configPath);
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
    m_trackList = j["tracks"].get<std::vector<AudioTrack>>();
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