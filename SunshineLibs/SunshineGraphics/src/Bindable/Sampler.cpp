

#include "Bindable/Sampler.h"
namespace Bind
{
	Sampler::Sampler(ID3D11Device* device, D3D11_SAMPLER_DESC samplerDesc, UINT slot, Bind::PipelineStage pipelineStage)
		: slot(slot), pipelineStage(pipelineStage)
	{
		device->CreateSamplerState(&samplerDesc, &pSampler);
	}
	void Sampler::Bind(ID3D11DeviceContext* context) noexcept
	{
		if (pipelineStage == Bind::PipelineStage::PIXEL_SHADER)
			context->PSSetSamplers(slot, 1u, pSampler.GetAddressOf());
		else if (pipelineStage == Bind::PipelineStage::COMPUTE_SHADER)
			context->CSSetSamplers(slot, 1u, pSampler.GetAddressOf());
	}

}
