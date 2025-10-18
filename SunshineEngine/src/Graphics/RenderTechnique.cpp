#include "Graphics/RenderTechnique.h"
#include "Bindable/Bindable.h"

using namespace Bind;

RenderTechnique::RenderTechnique(ID3D11Device* device, eastl::string technique)
	: techniqueTag(technique)
{
	D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rasterizer = eastl::make_shared<Bind::Rasterizer>(device, rastDesc);

	D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilState = eastl::make_shared<Bind::DepthStencilState>(device, dsDesc);
}

void RenderTechnique::AddBind(Bind::Bindable* bind)
{
	bindables.push_back(bind);
}

void RenderTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	BindAll(context);
	DrawTechnique(context);
}

void RenderTechnique::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	for (size_t i = 0; i < bindables.size(); i++)
	{
		bindables[i]->Bind(context.Get());
	}

	if (vertexShader)
		vertexShader->Bind(context.Get());

	if (pixelShader)
		pixelShader->Bind(context.Get());
	
	if (texture) {
		texture->Bind(context.Get());
	}

	if (textureSampler) {
		textureSampler->Bind(context.Get());
	}

	// Bind rasterizer
	rasterizer->Bind(context.Get());

	// Bind depthState
	depthStencilState->Bind(context.Get());

	if (blendState)
		blendState->Bind(context.Get());
}

void RenderTechnique::DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	if (mesh)
		mesh->Draw(context.Get());

}

eastl::string RenderTechnique::GetTechnique()
{
	return techniqueTag;
}
