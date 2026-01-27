#include "Audio/AudioSystem.h"
#include <fstream>
#include <Scripting/AutoBindings.h>

AudioSystem* AudioSystem::s_instance = nullptr;

void AudioSystem::Initialize() {
    FMOD_RESULT result;
    result = FMOD::System_Create(&m_system);
    CheckError(result);

    result = m_system->init(512, FMOD_INIT_NORMAL, 0);
    CheckError(result);
}

void AudioSystem::Shutdown() {
    if (m_system) {
        for (auto& pair : m_soundBank) {
            pair.second->release();
        }
        m_soundBank.clear();
        m_system->close();
        m_system->release();
        m_system = nullptr;
    }
}

void AudioSystem::Update() {
    if (m_system) {
        m_system->update();
    }
    
    for (auto it = m_activeChannels.begin(); it != m_activeChannels.end();) {
        bool isPlaying = false;
        it->second->isPlaying(&isPlaying);
        if (!isPlaying) {
            it = m_activeChannels.erase(it);
        } else {
            ++it;
        }
    }
}

void AudioSystem::LoadFromJson(const std::string& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open()) return;

    json j;
    file >> j;

    if (j.contains("tracks")) {
        std::vector<AudioTrack> tracks = j["tracks"].get<std::vector<AudioTrack>>();
        
        for (const auto& track : tracks) {
            m_tracksData[track.name] = track;

            FMOD_MODE mode = FMOD_DEFAULT;
            if (track.loop) mode |= FMOD_LOOP_NORMAL;
            else mode |= FMOD_LOOP_OFF;

            FMOD::Sound* sound = nullptr;
            FMOD_RESULT res = m_system->createSound(track.filePath.c_str(), mode, 0, &sound);
            
            if (res == FMOD_OK) {
                m_soundBank[track.name] = sound;
            } else {
                CheckError(res);
            }
        }
    }
}

AudioHandle AudioSystem::Play(const std::string& trackName, float volume, bool loop) {
    AudioHandle handle = { trackName, 0, false };

    if (m_soundBank.find(trackName) == m_soundBank.end()) {
        return handle;
    }

    FMOD::Sound* sound = m_soundBank[trackName];
    AudioTrack& config = m_tracksData[trackName];
    
    FMOD::Channel* channel = nullptr;
    
    FMOD_RESULT res = m_system->playSound(sound, 0, true, &channel);
    CheckError(res);

    if (res == FMOD_OK && channel) {
        channel->setVolume(volume);
        
        channel->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
        
        channel->setPaused(false);

        m_activeChannels[trackName] = channel;
        
        handle.isPlaying = true;
    }

    return handle;
}

void AudioSystem::Stop(const std::string& trackName) {
    if (m_activeChannels.count(trackName)) {
        m_activeChannels[trackName]->stop();
        m_activeChannels.erase(trackName);
    }
}

void AudioSystem::StopAll() {
    for (auto& pair : m_activeChannels) {
        pair.second->stop();
    }
    m_activeChannels.clear();
}

void AudioSystem::Pause(const std::string& trackName, bool pauseState) {
    if (m_activeChannels.count(trackName)) {
        m_activeChannels[trackName]->setPaused(pauseState);
    }
}

void AudioSystem::SetVolume(const std::string& trackName, float volume) {
    if (m_activeChannels.count(trackName)) {
        m_activeChannels[trackName]->setVolume(volume);
    }
}

bool AudioSystem::IsPlaying(const std::string& trackName) {
    if (m_activeChannels.count(trackName)) {
        bool isPlaying = false;
        m_activeChannels[trackName]->isPlaying(&isPlaying);
        return isPlaying;
    }
    return false;
}

AudioHandle AudioSystem::Play3D(const std::string& trackName, float x, float y, float z, 
                               float volume, float minDist, float maxDist) {
    AudioHandle handle = { trackName, 0, false };
    
    if (m_soundBank.find(trackName) == m_soundBank.end()) {
        return handle;
    }
    
    FMOD::Sound* sound = m_soundBank[trackName];
    AudioTrack& config = m_tracksData[trackName];
    
    FMOD::Channel* channel = nullptr;
    FMOD_RESULT res = m_system->playSound(sound, 0, true, &channel);
    CheckError(res);
    
    if (res == FMOD_OK && channel) {
        FMOD_VECTOR pos = { x, y, z };
        FMOD_VECTOR vel = { 0, 0, 0 };
        channel->set3DAttributes(&pos, &vel);
        
        channel->set3DMinMaxDistance(minDist, maxDist);
        
        channel->setVolume(volume * config.volume);
        channel->setMode(config.loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
        channel->setPaused(false);
        
        std::string channelKey = trackName + "_3d_" + std::to_string(rand());
        m_activeChannels[channelKey] = channel;
        
        handle.isPlaying = true;
        handle.instanceId = rand();
    }
    
    return handle;
}

void AudioSystem::SetSourcePosition(const std::string& trackName, float x, float y, float z) {
    for (auto& pair : m_activeChannels) {
        if (pair.first.find(trackName) != std::string::npos) {
            FMOD_VECTOR pos = { x, y, z };
            FMOD_VECTOR vel = { 0, 0, 0 };
            pair.second->set3DAttributes(&pos, &vel);
        }
    }
}

void AudioSystem::SetListenerPosition(float x, float y, float z) {
    if (!m_system) return;
    
    FMOD_VECTOR pos = { x, y, z };
    FMOD_VECTOR vel = { 0, 0, 0 };
    FMOD_VECTOR forward = { 0, 0, 1 };
    FMOD_VECTOR up = { 0, 1, 0 };
    
    m_system->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
}

void AudioSystem::MuteAll(bool mute) {
    for (auto& pair : m_activeChannels) {
        pair.second->setMute(mute);
    }
}

#define ADD_METHOD(k, fn) k, fn

LUA_REGISTER_TYPE(
    AudioSystem,
    "AudioSystem",
    /* no fields */ ,
    AUDIOSYSTEM_LUA_METHODS_APPLY(ADD_METHOD))

#undef ADD_METHOD