#include "Graphics/Lighting/AmbientLight.h"

namespace SE_G {
    AmbientLight::AmbientLight(AmbientLightData initData)
    {
        m_lightData = eastl::make_shared<AmbientLightData>(initData);
    }
}