#pragma once
#include "Component/Component.h"
#include <string>
#include <Utils/UUID.h>

class GameObject;

class AudioComponent : public Component {
public:
    std::string trackName;
    float volume = 1.0f;
    bool loop = false;
    bool playOnStart = false;
    bool spatial = false;  // 3D 
    float minDistance = 5.0f;
    float maxDistance = 50.0f;

    SE::UUID owner;
};

