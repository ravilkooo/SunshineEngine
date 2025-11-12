#include "Graphics/Lighting/AmbientLight.h"

namespace SE_G {
    AmbientLight::AmbientLight(AmbientLightData initData)
    {
        m_lightData = eastl::make_shared<AmbientLightData>(initData);
    }

    AmbientLight_Info::AmbientLight_Info(AmbientLightData initData)
    {
        m_lightData = eastl::make_shared<AmbientLightData>(initData);
        m_name = "AmbientLight";
        m_group = GameObjectGroup::Lighting;
    }
}