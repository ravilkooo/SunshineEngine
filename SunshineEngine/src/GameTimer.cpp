#include "GameTimer.h"

GameTimer::GameTimer() : deltaTime(0.0f)
{
    lastTime = std::chrono::high_resolution_clock::now();
}

void GameTimer::Tick()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;
}

float GameTimer::GetDeltaTime() const
{
    return deltaTime;
}