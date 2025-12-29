#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>



using json = nlohmann::json;

struct AudioTrack {
    std::string id;
    std::string filePath;
    std::string tag;        // "ambient", "sfx", "music"
    bool loop = false;
    float volume = 1.0f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AudioTrack, id, filePath, tag, loop, volume)

struct AudioHandle {
    std::string trackId;
    unsigned int instanceId;
    bool isPlaying;
};

class AudioSystem {
private:
    FMOD::System* m_system = nullptr;
    
    std::map<std::string, FMOD::Sound*> m_soundBank;
    
    std::map<std::string, AudioTrack> m_tracksData;

    std::map<std::string, FMOD::Channel*> m_activeChannels;

    void CheckError(FMOD_RESULT result) {
        if (result != FMOD_OK) {
            // LOG_EDITOR_ERROR("FMOD Error: %d - %s\n", result, FMOD_ErrorString(result));
        }
    }

public:
    AudioSystem();
    ~AudioSystem();

    void Initialize();
    void Update();
    void Shutdown();

    void LoadFromJson(const std::string& jsonPath);

    AudioHandle Play(const std::string& trackId);
    void Stop(const std::string& trackId);
    void StopAll(); 
    void Pause(const std::string& trackId, bool pauseState);
    void SetVolume(const std::string& trackId, float volume);
};