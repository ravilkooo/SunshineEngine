#pragma once

#include "SunshineEngineAPI.h"

#include <chrono>

class SUNSHINE_ENGINE_API GameTimer
{
public:
    GameTimer();

    void Tick();
    float GetDeltaTime() const;

private:
    std::chrono::high_resolution_clock::time_point lastTime;
    float deltaTime;
};
