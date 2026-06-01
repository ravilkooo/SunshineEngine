#pragma once

#include <string>

enum class InputActionPhase
{
    None,
    Pressed,
    Released,
    Held
};

struct ActionState
{
    bool Pressed = false;
    bool Released = false;
    bool Held = false;

    uint32_t PressCount = 0;
};

struct KeyState
{
    bool Pressed = false;
    bool Released = false;
    bool Held = false;
};

struct AxisMapping
{
    std::string Name;
    float Scale = 1.0f;
};

struct AxisState
{
    float Value = 0.0f;
};

struct MouseState
{
    float DeltaX = 0.0f;
    float DeltaY = 0.0f;

    float PositionX = 0.0f;
    float PositionY = 0.0f;

    float WheelDelta = 0.0f;
};
