#pragma once
#include "Audio/AudioSystem.h"


class AudioEditor {
private:
    std::vector<AudioTrack> m_trackList;
    AudioSystem* m_previewSystem;
    static std::string s_configPath;

public:
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

    void SetConfigPath(const std::string& path);
    static const std::string& GetConfigPath();

    AudioTrack* getTrack(std::string name);
    void AddTrack(const AudioTrack& track);
    void RemoveTrack(std::string name);
    const std::vector<AudioTrack>& GetTrackList() const { return m_trackList; }
    
    void PlayPreview(std::string name, float volume = 1.0f, bool loop = false);
    void StopPreview();
    
    void SetLoop(std::string name, bool loop);
    void SetVolume(std::string name, float volume);
    
    void SaveToJson();
    void SaveToJson(const std::string& path);
    bool LoadFromJson();
    
    void RenameTrack(const std::string& oldName, const std::string& newName);
    void AddTrackWithPath(const std::string& name, const AssetPath& path, 
                         const std::string& tag = "sfx", 
                         bool loop = false, float volume = 1.0f);
};