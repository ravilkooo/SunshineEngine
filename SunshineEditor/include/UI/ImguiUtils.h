#pragma once

#include <imgui.h>

#include <string>

#include <SimpleMath.h>
namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

class ImguiUtils
{
public:
    static bool DrawVector3Control(const char* label, DXSM::Vector3& values,
        DXSM::Vector3 minValues, DXSM::Vector3 maxValues,
        float resetValue = 0.0f, float columnWidth = 100.0f);

    static bool DrawVector2Control(const char* label, DXSM::Vector2& values,
        float resetValue = 0.0f, float columnWidth = 100.0f);

    static bool InputString(
        const char* label,
        std::string& value);
};