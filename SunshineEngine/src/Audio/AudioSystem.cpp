#include "Audio/AudioSystem.h"
#include <fstream>

AudioSystem::AudioSystem() {
    Initialize();
}

AudioSystem::~AudioSystem() {
    Shutdown();
}

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
            m_tracksData[track.id] = track;

            FMOD_MODE mode = FMOD_DEFAULT;
            if (track.loop) mode |= FMOD_LOOP_NORMAL;
            else mode |= FMOD_LOOP_OFF;

            FMOD::Sound* sound = nullptr;
            FMOD_RESULT res = m_system->createSound(track.filePath.c_str(), mode, 0, &sound);
            
            if (res == FMOD_OK) {
                m_soundBank[track.id] = sound;
            } else {
                CheckError(res);
            }
        }
    }
}

AudioHandle AudioSystem::Play(const std::string& trackId) {
    AudioHandle handle = { trackId, 0, false };

    if (m_soundBank.find(trackId) == m_soundBank.end()) {
        return handle;
    }

    FMOD::Sound* sound = m_soundBank[trackId];
    AudioTrack& config = m_tracksData[trackId];
    
    FMOD::Channel* channel = nullptr;
    
    FMOD_RESULT res = m_system->playSound(sound, 0, true, &channel);
    CheckError(res);

    if (res == FMOD_OK && channel) {
        channel->setVolume(config.volume);
        
        channel->setMode(config.loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
        
        channel->setPaused(false);

        m_activeChannels[trackId] = channel;
        
        handle.isPlaying = true;
    }

    return handle;
}

void AudioSystem::Stop(const std::string& trackId) {
    if (m_activeChannels.count(trackId)) {
        m_activeChannels[trackId]->stop();
        m_activeChannels.erase(trackId);
    }
}

void AudioSystem::StopAll() {
    for (auto& pair : m_activeChannels) {
        pair.second->stop();
    }
    m_activeChannels.clear();
}

void AudioSystem::Pause(const std::string& trackId, bool pauseState) {
    if (m_activeChannels.count(trackId)) {
        m_activeChannels[trackId]->setPaused(pauseState);
    }
}

void AudioSystem::SetVolume(const std::string& trackId, float volume) {
    if (m_activeChannels.count(trackId)) {
        m_activeChannels[trackId]->setVolume(volume);
    }
}