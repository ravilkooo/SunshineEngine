#include "Graphics/LightTechnique.h"

template<class T>
void LightTechnique<T>::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	for (size_t i = 0; i < bindables.size(); i++)
	{
		bindables[i]->Bind(context.Get());
	}

	if (vertexShader) {
		vertexShader->Bind(context.Get());
	}

	if (pixelShader) {
		pixelShader->Bind(context.Get());
	}

	if (texture) {
		texture->Bind(context.Get());
	}

	if (textureSampler) {
		textureSampler->Bind(context.Get());
	}

	if (lightDataBuffer) {
		lightDataBuffer->Bind(context.Get());
	}

	if (blendState)
		blendState->Bind(context.Get());

	LightPosition lightPos = GetLightPositionInFrustum();
	// Choose rasterizer
	ChooseRasterizer(lightPos);
	// Choose depthState
	ChooseDepthStencilState(lightPos);

	// Bind rasterizer
	rasterizer->Bind(context.Get());
	
	// Bind depthState
	depthStencilState->Bind(context.Get());
}

template <class T>
LightTechnique<T>::LightTechnique(ID3D11Device* device, eastl::string technique)
	: RenderTechnique(device, technique)
{
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendState = eastl::make_shared<Bind::BlendState>(device, blendDesc);
}
