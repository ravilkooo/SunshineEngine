#pragma once
#include <fstream>
#include <algorithm>
#include <set>
#include "Audio/AudioSystem.h"
#include "Utils/FileCategories.h"

class AudioEditor {
private:
    std::vector<AudioTrack> m_trackList;
    AudioSystem* m_previewSystem;
    std::string m_configPath;

    float m_scanTimer = 0.0f;
    const float SCAN_INTERVAL = 2.0f;

public:
    AudioEditor(AudioSystem* audioSystem) : m_previewSystem(audioSystem) {}

    void SetAudioSystem(AudioSystem* system) { m_previewSystem = system; }
    AudioSystem* GetAudioSystem() const { return m_previewSystem; }

    void Update(float deltaTime, const std::filesystem::path& rootDirectory) {
        if (m_previewSystem) {
            m_previewSystem->Update();
        }

        m_scanTimer += deltaTime;
        if (m_scanTimer >= SCAN_INTERVAL) {
            ScanAndSync(rootDirectory);
            m_scanTimer = 0.0f;
        }
    }
    std::string PathToUtf8(const std::filesystem::path& path) {
#if defined(_WIN32) && defined(__cpp_lib_char8_t)
        const auto u8str = path.u8string();
        return std::string(reinterpret_cast<const char*>(u8str.c_str()), u8str.length());
#else
        return path.u8string(); 
#endif
    }
    
    void ScanAndSync(const std::filesystem::path& rootDirectory) {
        namespace fs = std::filesystem;
    
        std::error_code ec;
        if (!fs::exists(rootDirectory, ec)) return;

        std::set<std::string> validExtensions;
        const auto& categories = FileCategories::Get(); 
        for (const auto& cat : categories) {
            if (cat.name == "Audio") {
                for (const auto& ext : cat.patterns)
                {
                    std::string lowerExt = ext;
                    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
                    validExtensions.insert(ext);
                }
                break;
            }
        }
        if (validExtensions.empty()) return;

        std::set<fs::path> filesOnDiskPaths;

        try {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(rootDirectory)) {
                if (entry.is_regular_file()) {
                    std::string ext = entry.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), 
                    [](unsigned char c){ return std::tolower(c); });

                    if (validExtensions.count(ext)) {
                        filesOnDiskPaths.insert(entry.path().generic_string());
                    }
                }
            }
        } catch (...) { return; }

        std::set<std::string> filesOnDiskUtf8;
        for(const auto& p : filesOnDiskPaths) {
            filesOnDiskUtf8.insert(PathToUtf8(p));
        }
        
        auto it = std::remove_if(m_trackList.begin(), m_trackList.end(), 
            [&](const AudioTrack& t) {
                return filesOnDiskUtf8.find(t.filePath) == filesOnDiskUtf8.end();
            });
        if (it != m_trackList.end()) m_trackList.erase(it, m_trackList.end());

        std::set<std::string> currentTrackPaths;
        for (const auto& t : m_trackList) currentTrackPaths.insert(t.filePath);

        for (const auto& diskPathUtf8 : filesOnDiskUtf8) {
            if (currentTrackPaths.find(diskPathUtf8) == currentTrackPaths.end()) {
                AddTrack(diskPathUtf8); 
            }
        }
    }
    
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
        m_configPath = jsonPath;
        std::ifstream file(jsonPath);
        if (!file.is_open()) return;
        /*json j;
        try {
            file >> j;
            if (j.contains("tracks")) {
                m_trackList = j["tracks"].get<std::vector<AudioTrack>>();
            }
        } catch (...) {}*/
        
        m_trackList.clear();

        try {
            json j;
            file >> j;

            if (j.contains("tracks") && j["tracks"].is_array()) {
            
                for (const auto& item : j["tracks"]) {
                    AudioTrack track;

                    if (item.contains("id")) 
                        track.id = item["id"].get<std::string>();
                
                    if (item.contains("filePath")) 
                        track.filePath = item["filePath"].get<std::string>();
                
                    if (item.contains("tag")) 
                        track.tag = item["tag"].get<std::string>();

                    track.loop = item.value("loop", false);
                    track.volume = item.value("volume", 1.0f);

                    m_trackList.push_back(track);
                }
            }
        }
        catch (const std::exception& e) {
            LOG_EDITOR_ERROR("JSON Load Error: %s\n", e.what());
        }
    }

    void Save() {
        if (!m_configPath.empty()) {
            SaveToJson(m_configPath);
        }
    }
};