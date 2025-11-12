#include "GameObject/Lighting/DirectionalLight.h"

DirectionalLight::DirectionalLight(
    SE_G::DirectionalLightData directionalLightData)
{
    directionalLightData.Direction.Normalize();

    m_lightData = eastl::make_shared<SE_G::DirectionalLightData>(directionalLightData);
}

DirectionalLight_Info::DirectionalLight_Info(
    SE_G::DirectionalLightData directionalLightData)
{
    directionalLightData.Direction.Normalize();

    m_lightData = eastl::make_shared<SE_G::DirectionalLightData>(directionalLightData);
    m_name = "DirectionalLight";
    m_group = GameObjectGroup::Lighting;
}