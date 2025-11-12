#include "Graphics/Lighting/AmbientLight.h"

AmbientLight::AmbientLight(SE_G::AmbientLightData initData)
{
    m_lightData = eastl::make_shared<SE_G::AmbientLightData>(initData);
}

AmbientLight_Info::AmbientLight_Info(SE_G::AmbientLightData initData)
{
    m_lightData = eastl::make_shared<SE_G::AmbientLightData>(initData);
    m_name = "AmbientLight";
    m_group = GameObjectGroup::Lighting;
}