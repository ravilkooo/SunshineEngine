#include "Graphics/Lighting/AmbientLight.h"

AmbientLight::AmbientLight(AmbientLightData initData)
{
    ambientLightData = eastl::make_shared<AmbientLightData>(initData);
}
