#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <EASTL/shared_ptr.h>

#include "GameObject.h"

#include "LightObject.h"
#include "LightData.h"

namespace DXSM = DirectX::SimpleMath;

namespace SE_G {
    class AmbientLight :
        public LightObject<AmbientLightData>
    {
    public:
        AmbientLight(AmbientLightData initData = { DXSM::Vector3::One, 1.0f });
    };

    class AmbientLight_Info :
        public LightObject_Info<AmbientLightData>
    {
    public:
        AmbientLight_Info(AmbientLightData initData = { DXSM::Vector3::One, 1.0f });

        // Serialization
        json ToJson() const override;
        static eastl::unique_ptr<AmbientLight_Info> FromJson(const json& j);
    };
}
