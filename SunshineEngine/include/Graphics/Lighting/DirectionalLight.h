#pragma once


#include <d3d11.h>
#include <SimpleMath.h>
#include <EASTL/shared_ptr.h>
#include <Graphics/GraphicsResources/VertexShader.h>
#include "GameObject.h"

#include "LightObject.h"
#include "LightData.h"

namespace DXSM = DirectX::SimpleMath;

namespace SE_G {
    class DirectionalLight :
        public LightObject<DirectionalLightData>
    {
    public:

        DirectionalLight(
            DirectionalLightData directionalLightData =
            {
                DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
                DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
                DXSM::Vector3::Zero, 0,
                DXSM::Vector3(0, -1, 0), 0
            });
    };

    class DirectionalLight_Info :
        public LightObject_Info<DirectionalLightData>
    {
    public:

        DirectionalLight_Info(
            DirectionalLightData directionalLightData =
            {
                DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
                DXSM::Vector3(250.0f / 255.0f, 222.0f / 255.0f, 133.0f / 255.0f), 1.0f,
                DXSM::Vector3::Zero, 0,
                DXSM::Vector3(0, -1, 0), 0
            });

        // Serialization
        json ToJson() const override;
        static eastl::unique_ptr<DirectionalLight_Info> FromJson(const json& j);
    };
}
