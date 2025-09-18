#pragma once



#include "GraphicsUtils/SceneNode.h"
#include "GraphicsUtils/Camera.h"

class LightObject :
    public SceneNode
{
public:
    enum class LightPosition {
        INSIDE, INTERSECT_FAR_PLANE, OUTSIDE, FILL, BEHIND_NEAR_PLANE
    };
    
    virtual D3D11_DEPTH_STENCIL_DESC GetDepthStencilDesc(LightObject::LightPosition lightPos)
    {
        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        return dsDesc;
    };
    virtual D3D11_RASTERIZER_DESC GetRasterizerDesc(LightObject::LightPosition lightPos)
    {
        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        return rasterDesc;
    };

    virtual LightPosition GetLightPositionInFrustum(Camera* camera) { return LightPosition::FILL; };
    virtual bool IsFrustumInsideOfLight(Camera* camera) { return true; };

    virtual void UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) = 0;
};

