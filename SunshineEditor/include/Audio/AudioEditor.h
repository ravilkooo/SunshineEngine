#pragma once
#include "Audio/AudioSystem.h"
#include <Utils/AssetPath.h>
#include <EASTL/string.h>

class AudioEditor {
private:
    AudioSystem* m_previewSystem;

public:
    static void ScanAudioFiles();
    static std::vector<AssetPath> m_audioFiles;

    std::vector<AudioTrack> m_trackList;
    static eastl::wstring s_configPath;

    AudioEditor(AudioSystem* audioSystem = nullptr) : m_previewSystem(audioSystem)
    {
        if (!m_previewSystem && AudioSystem::IsInitialized()) {
            m_previewSystem = &AudioSystem::Get();
        }
    }

    void SetAudioSystem(AudioSystem* system)
    {
        m_previewSystem = system;
        if (!m_previewSystem && AudioSystem::IsInitialized()) {
            m_previewSystem = &AudioSystem::Get();
        }
    }
    AudioSystem* GetAudioSystem() const { return m_previewSystem; }

    void Update();

    static void SetConfigPath(const eastl::wstring& path);
    static const eastl::wstring& GetConfigPath();

    AudioTrack* getTrack(std::string name);
    AudioTrack* getTrack(AssetPath name);
    void AddTrack(const AudioTrack& track);
    void RemoveTrack(std::string name);
    const std::vector<AudioTrack>& GetTrackList() const { return m_trackList; }
    
    void PlayPreview(std::string name);
    void StopPreview();
    
    void SetLoop(std::string name, bool loop);
    void SetVolume(std::string name, float volume);
    
    void SaveToJson();
    void SaveToJson(const eastl::wstring& path);
    bool LoadFromJson();
    
    void RenameTrack(const std::string& oldName, const std::string& newName);
};