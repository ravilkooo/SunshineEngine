#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include <Utils/AssetPath.h>


using json = nlohmann::json;

struct AudioTrack {
    std::string name;
    AssetPath filePath;
    std::string tag;        // "ambient", "sfx", "music"
    bool loop = false;
    float volume = 1.0f;

    void FromJson(const json& j)
    {
        this->name = j["name"].get<std::string>();
        this->filePath.FromJson(j["filePath"]);
        this->tag = j["tag"].get<std::string>();
        this->loop = j["loop"].get<bool>();
        this->volume = j["volume"].get<float>();
    }

    json ToJson() const
    {
        json j;
        j["name"] = name;
        j["filePath"] = filePath.ToJson();
        j["tag"] = tag;
        j["loop"] = loop;
        j["volume"] = volume;
        return j;
    }

    inline void to_json(json& j, const AudioTrack& v) {
        j = v.ToJson();
    }
    inline void from_json(const json& j, AudioTrack& v) {
        v.FromJson(j);
    }
};

struct AudioHandle {
    std::string trackName;
    unsigned int instanceId;
    bool isPlaying;
};

class AudioSystem {
private:
    FMOD::System* m_system = nullptr;
    
    std::map<std::string, FMOD::Sound*> m_soundBank;
    
    std::map<std::string, AudioTrack> m_tracksData;

    std::map<std::string, FMOD::Channel*> m_activeChannels;

    static AudioSystem* s_instance;

    AudioSystem() = default;
    ~AudioSystem() { Shutdown(); }

    void CheckError(FMOD_RESULT result) {
        if (result != FMOD_OK) {
            // LOG_EDITOR_ERROR("FMOD Error: %d - %s\n", result, FMOD_ErrorString(result));
            std::cout << FMOD_ErrorString(result);
        }
    }

public:
    static AudioSystem& Get() {
        if (!s_instance) {
            s_instance = new AudioSystem();
            s_instance->Initialize();
        }
        return *s_instance;
    }
    
    static void Destroy() {
        if (s_instance) {
            delete s_instance;
            s_instance = nullptr;
        }
    }

    static bool IsInitialized() { return s_instance != nullptr; }
    
    void Initialize();
    void Update();
    void Shutdown();

    void LoadFromJson(const std::string& jsonPath);

    AudioHandle Play(const std::string& trackName, float volume = 1.0f, bool loop = false);
    void Stop(const std::string& trackName);
    void StopAll(); 
    void Pause(const std::string& trackName, bool pauseState);
    void SetVolume(const std::string& trackName, float volume);
    bool IsPlaying(const std::string& trackName);

    AudioHandle Play3D(const std::string& trackName, float x, float y, float z, 
                      float volume = 1.0f, float minDist = 5.0f, float maxDist = 50.0f);
    void SetSourcePosition(const std::string& trackName, float x, float y, float z);
    void SetListenerPosition(float x, float y, float z);
    
    void MuteAll(bool mute);

};

#ifndef AUDIOSYSTEM_LUA_METHODS_APPLY
#define AUDIOSYSTEM_LUA_METHODS_APPLY(FM) \
    FM("play", [](AudioSystem* self, const std::string& name) { \
        return self->Play(name); \
    }) , \
    FM("playWithParams", [](AudioSystem* self, const std::string& name, sol::optional<float> vol, sol::optional<bool> loop) { \
        return self->Play(name, vol.value_or(1.0f), loop.value_or(false)); \
    }) , \
    FM("play3D", [](AudioSystem* self, const std::string& name, float x, float y, float z, \
        sol::optional<float> vol, sol::optional<float> minDist, sol::optional<float> maxDist) { \
        return self->Play3D(name, x, y, z, vol.value_or(1.0f), minDist.value_or(5.0f), maxDist.value_or(50.0f)); \
    }) , \
    FM("stop", &AudioSystem::Stop) , \
    FM("stopAll", &AudioSystem::StopAll) , \
    FM("pause", &AudioSystem::Pause) , \
    FM("setVolume", &AudioSystem::SetVolume) , \
    FM("isPlaying", &AudioSystem::IsPlaying) , \
    FM("setSourcePosition", &AudioSystem::SetSourcePosition) , \
    FM("setListenerPosition", &AudioSystem::SetListenerPosition) , \
    FM("muteAll", &AudioSystem::MuteAll)
#endif