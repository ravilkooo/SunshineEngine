#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <Graphics/GraphicsResources/VertexShader.h>
#include "GameObject/GameObject.h"

namespace DXSM = DirectX::SimpleMath;


namespace SE_G {
    class FullScreenQuad : public GameObject
    {

        /*
    public:
        FullScreenQuad();
        FullScreenQuad(ID3D11Device* device);

        void Update(float deltaTime) {};

        DXSM::Vector3 GetCenterLocation() { return DXSM::Vector3::Zero;  };
    private:
        Bind::VertexShader* vertexShaderB = nullptr;
        */
    };
}
