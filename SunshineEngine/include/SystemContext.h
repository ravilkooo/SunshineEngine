#pragma once

class Scene;
class PhysicsSystem;
class GrabSystem;
class PlayerInputSystem;
class CharacterControllerSystem;
class AudioSystem;

namespace SE
{
    class ParticleSystem;
}

namespace SE_G
{
    class DeferredRenderer;
}

class SystemContext
{
public:
    SE_G::DeferredRenderer* renderer = nullptr;
    SE::ParticleSystem* particle = nullptr;
    Scene* scene = nullptr;
    PhysicsSystem* physics = nullptr;
    CharacterControllerSystem* characterController = nullptr;
    GrabSystem* grab = nullptr;
    PlayerInputSystem* input = nullptr;
    AudioSystem* audio = nullptr;
};
