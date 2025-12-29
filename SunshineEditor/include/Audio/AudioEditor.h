#pragma once
#include <fstream>
#include <algorithm>
#include "Audio/AudioSystem.h" 

class AudioEditor {
private:
    std::vector<AudioTrack> m_trackList;
    AudioSystem* m_previewSystem;

public:
    AudioEditor(AudioSystem* audioSystem) : m_previewSystem(audioSystem) {}

    void SetAudioSystem(AudioSystem* system) { m_previewSystem = system; }

    void AddTrack(std::string filePath) {
        AudioTrack newTrack;
        newTrack.filePath = filePath;
        size_t lastSlash = filePath.find_last_of("/\\");
        std::string filename = (lastSlash == std::string::npos) ? filePath : filePath.substr(lastSlash + 1);
        newTrack.id = filename; 
        newTrack.volume = 1.0f;
        newTrack.loop = false;
        newTrack.tag = "sfx";

        m_trackList.push_back(newTrack);
    }

    void RemoveTrack(std::string id) {
        auto it = std::remove_if(m_trackList.begin(), m_trackList.end(), 
            [&](const AudioTrack& t) { return t.id == id; });
        
        if (it != m_trackList.end()) {
            m_trackList.erase(it, m_trackList.end());
        }
    }

    const std::vector<AudioTrack>& GetTrackList() const { return m_trackList; }

    void PlayPreview(std::string id) {
        if (!m_previewSystem) return;
        SaveToJson("temp_audio_preview.json");
        m_previewSystem->LoadFromJson("temp_audio_preview.json");
        
        m_previewSystem->Play(id);
    }

    void StopPreview() {
        if (m_previewSystem) m_previewSystem->StopAll();
    }

    void SetLoop(std::string id, bool loop) {
        AudioTrack* t = getTrack(id);
        if (t) t->loop = loop;
    }

    void SetVolume(std::string id, float volume) {
        AudioTrack* t = getTrack(id);
        if (t) t->volume = volume;
    }

    AudioTrack* getTrack(std::string id) {
        for (auto& t : m_trackList) {
            if (t.id == id) return &t;
        }
        return nullptr;
    }

    void SaveToJson(std::string outputPath) {
        json j;
        j["tracks"] = m_trackList;
        std::ofstream file(outputPath);
        if (file.is_open()) {
            file << j.dump(4);
        }
    }

    void LoadFromJson(std::string jsonPath) {
        std::ifstream file(jsonPath);
        if (!file.is_open()) return;
        json j;
        try {
            file >> j;
            if (j.contains("tracks")) {
                m_trackList = j["tracks"].get<std::vector<AudioTrack>>();
            }
        } catch (...) {}
    }
};