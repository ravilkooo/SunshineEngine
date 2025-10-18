#include "Graphics/SkyBoxTechnique.h"

SkyBoxTechnique::SkyBoxTechnique(ID3D11Device* device, eastl::string technique)
    : LightTechnique(device, technique) {

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilState = eastl::make_shared<Bind::DepthStencilState>(device, dsDesc);

    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.CullMode = D3D11_CULL_FRONT;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterizer = eastl::make_shared<Bind::Rasterizer>(device, rasterDesc);

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    blendState = eastl::make_shared<Bind::BlendState>(device, blendDesc);
}

void SkyBoxTechnique::ChooseDepthStencilState(LightPosition lightPos)
{
    return;
}

void SkyBoxTechnique::ChooseRasterizer(LightPosition lightPos)
{
    return;
}

LightPosition SkyBoxTechnique::GetLightPositionInFrustum()
{
	return LightPosition::FILL;
}

bool SkyBoxTechnique::IsFrustumInsideOfLight()
{
    return true;
}

