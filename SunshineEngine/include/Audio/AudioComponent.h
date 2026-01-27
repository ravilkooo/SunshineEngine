#pragma once
#include "Component/Component.h"
#include <string>

class AudioComponent : public Component {
public:
    std::string trackName;
    float volume = 1.0f;
    bool loop = false;
    bool playOnStart = false;
    bool spatial = false;  // 3D 
    float minDistance = 5.0f;
    float maxDistance = 50.0f;
    
#ifndef AUDIOCOMP_LUA_FIELDS_APPLY
#define AUDIOCOMP_LUA_FIELDS_APPLY(F) \
    F(trackName) , \
    F(volume) , \
    F(loop) , \
    F(playOnStart) , \
    F(spatial) , \
    F(minDistance) , \
    F(maxDistance)
#endif

#ifndef AUDIOCOMP_LUA_METHODS_APPLY
#define AUDIOCOMP_LUA_METHODS_APPLY(FM) \
    FM("play", [](AudioComponent* self) { \
        // auto* audio = AudioSystem::Get(); \
        if (self->spatial && self->owner) { \
            auto pos = self->owner->GetTransform()->GetPosition(); \
            audio->Play3D(self->trackName, pos.x, pos.y, pos.z, \
                            self->volume, self->minDistance, self->maxDistance); \
        } else { \
            audio->Play(self->trackName, self->volume, self->loop); \
        } \
    }) , \
    FM("stop", [](AudioComponent* self) { \
        AudioSystem::Get()->Stop(self->trackName); \
    }) , \
    FM("setVolume", [](AudioComponent* self, float v) { \
        self->volume = v; \
        AudioSystem::Get()->SetVolume(self->trackName, v); \
    }) , \
    FM("updatePosition", [](AudioComponent* self) { \
        if (self->spatial && self->owner) { \
            auto pos = self->owner->GetTransform()->GetPosition(); \
            AudioSystem::Get()->SetSourcePosition(self->trackName, pos.x, pos.y, pos.z); \
        } \
    })
#endif
};