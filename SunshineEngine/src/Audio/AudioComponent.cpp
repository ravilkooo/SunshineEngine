#include <Audio/AudioComponent.h>
#include <Component/TransformComponent.h>
#include <GameObject/GameObject.h>
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>
#include "Audio/AudioSystem.h"
#include <Scene.h>

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
        auto& audio = AudioSystem::Get(); \
        \
        if (self->spatial && self->owner != SE::UUID(0u)) { \
            auto go = Scene::GetInstance().GetGameObjectByUUID(self->owner); \
            auto pos = go->GetComponent<TransformComponent>()->GetAbsoluteWorldPosition(); \
            audio.Play3D( \
                self->trackName, \
                pos.x, pos.y, pos.z, \
                self->volume, \
                self->minDistance, \
                self->maxDistance \
            ); \
        } else { \
            audio.Play(self->trackName, self->volume, self->loop); \
        } \
    }) \
    FM("stop", [](AudioComponent* self) { \
        AudioSystem::Get().Stop(self->trackName); \
    }) \
    FM("setVolume", [](AudioComponent* self, float v) { \
        self->volume = v; \
        AudioSystem::Get().SetVolume(self->trackName, v); \
    }) \
    FM("updatePosition", [](AudioComponent* self) { \
        if (self->spatial && self->owner != SE::UUID(0u)) { \
            auto go = Scene::GetInstance().GetGameObjectByUUID(self->owner); \
            auto pos = go->GetComponent<TransformComponent>()->GetAbsoluteWorldPosition(); \
            AudioSystem::Get().SetSourcePosition( \
                self->trackName, pos.x, pos.y, pos.z \
            ); \
        } \
    })
#endif

#ifndef AUDIOCOMP_LUA_METHODS_APPLY
#define AUDIOCOMP_LUA_METHODS_APPLY(FM) \
    FM("play", [](AudioComponent* self) { \
        auto& audio = AudioSystem::Get(); \
        if (self->spatial && self->owner) { \
            auto pos = self->owner->GetTransform()->GetPosition(); \
            audio.Play3D( \
                    self->trackName, \
                    pos.x, pos.y, pos.z, \
                    self->volume, \
                    self->minDistance, \
                    self->maxDistance \
            ); \
        } else { \
            audio.Play(self->trackName, self->volume, self->loop); \
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
            AudioSystem::Get()->SetSourcePosition( \
                self->trackName, pos.x, pos.y, pos.z \
            ); \
        } \
    })
#endif


#define ADD_FIELD(name) #name, &AudioComponent::name
#define AUDIOCOMP_FIELD_PAIRS AUDIOCOMP_LUA_FIELDS_APPLY(ADD_FIELD)

#define ADD_METHOD_WITH_LEAD(k, fn) , k, fn
#define AUDIOCOMP_METHOD_PAIRS AUDIOCOMP_LUA_METHODS_APPLY(ADD_METHOD_WITH_LEAD)

LUA_REGISTER_COMPONENT(
    AudioComponent,
    "AudioComponent",
    AUDIOCOMP_FIELD_PAIRS,
    AUDIOCOMP_METHOD_PAIRS,
    "getAudio")

#undef ADD_FIELD
#undef ADD_METHOD_WITH_LEAD