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

    float Value = 0.0f;
};

struct AxisMapping
{
    std::string Name;
    float scale = 1.0f;
};
