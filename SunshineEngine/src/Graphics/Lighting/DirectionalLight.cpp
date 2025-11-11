#include "Graphics/Lighting/DirectionalLight.h"

namespace SE_G {
    DirectionalLight::DirectionalLight(
        DirectionalLightData directionalLightData)
    {
        directionalLightData.Direction.Normalize();

        m_lightData = eastl::make_shared<DirectionalLightData>(directionalLightData);
    }

    DirectionalLight_Info::DirectionalLight_Info(
        DirectionalLightData directionalLightData)
    {
        directionalLightData.Direction.Normalize();

        m_lightData = eastl::make_shared<DirectionalLightData>(directionalLightData);
    }
}