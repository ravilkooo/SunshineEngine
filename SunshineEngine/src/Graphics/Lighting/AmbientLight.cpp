#include "Graphics/Lighting/AmbientLight.h"

AmbientLight::AmbientLight(AmbientLightData initData)
{
    m_lightData = eastl::make_shared<AmbientLightData>(initData);
}
