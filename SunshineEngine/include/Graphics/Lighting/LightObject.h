#pragma once

#include "GameObject.h"

template <class T>
class LightObject : public GameObject
{
public:
    eastl::shared_ptr<T> m_lightData;
    // Unnecessary?
    //virtual void UpdateLightBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) = 0;

    /*
    virtual D3D11_DEPTH_STENCIL_DESC ChooseDepthStencilState(LightObject::LightPosition lightPos)
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
    */
};

/*
template class LightObject<AmbientLightData>;

template class LightObject<DirectionalLightData>;

template class LightObject<PointLightData>;

template class LightObject<SkyBoxData>;
*/