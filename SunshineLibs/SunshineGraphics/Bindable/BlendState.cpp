#include "BlendState.h"
namespace Bind
{
	BlendState::BlendState(ID3D11Device* device,
		D3D11_BLEND_DESC blendDesc,
		FLOAT* blendFactor, UINT sampleMask) :
		sampleMask(sampleMask)
	{
		device->CreateBlendState(&blendDesc, &pBlendState);
		if (blendFactor) {
			this->blendFactor[0] = blendFactor[0];
			this->blendFactor[1] = blendFactor[1];
			this->blendFactor[2] = blendFactor[2];
			this->blendFactor[3] = blendFactor[3];
		}
	}

	void BlendState::Bind(ID3D11DeviceContext* context) noexcept
	{
		context->OMSetBlendState(pBlendState.Get(), blendFactor, sampleMask);

	}

}