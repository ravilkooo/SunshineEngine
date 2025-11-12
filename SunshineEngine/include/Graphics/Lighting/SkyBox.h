#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <EASTL/shared_ptr.h>

#include <GameObject.h>
#include "LightObject.h"
#include "LightData.h"

namespace DXSM = DirectX::SimpleMath;

namespace SE_G {
    class SkyBox :
        public LightObject<SkyBoxData>
    {
    public:
        SkyBox(SkyBoxData initData = { DXSM::Vector3::One , 0.0f });
    };


    class SkyBox_Info :
        public LightObject_Info<SkyBoxData>
    {
    public:
        SkyBox_Info(SkyBoxData initData = { DXSM::Vector3::One , 0.0f });

        // Serialization
        json ToJson() const override;
        static eastl::unique_ptr<SkyBox_Info> FromJson(const json& j);
    };
}
