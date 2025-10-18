#include "Graphics/DirectionalLightTechnique.h"

DirectionalLightTechnique::DirectionalLightTechnique(ID3D11Device* device, eastl::string technique)
    : LightTechnique(device, technique) {

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilState = eastl::make_shared<Bind::DepthStencilState>(device, dsDesc);

    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterizer = eastl::make_shared<Bind::Rasterizer>(device, rasterDesc);
}

void DirectionalLightTechnique::ChooseDepthStencilState(LightPosition lightPos)
{
    return;
}

void DirectionalLightTechnique::ChooseRasterizer(LightPosition lightPos)
{
    return;
}

LightPosition DirectionalLightTechnique::GetLightPositionInFrustum()
{
	return LightPosition::FILL;
}

bool DirectionalLightTechnique::IsFrustumInsideOfLight()
{
	return true;
}


