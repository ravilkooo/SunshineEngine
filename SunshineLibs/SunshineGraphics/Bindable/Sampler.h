#pragma once



#include "Bindable.h"

namespace Bind
{
	class Sampler :
		public Bindable
	{
	public:
		Sampler(ID3D11Device* device,
			D3D11_SAMPLER_DESC  samplerDesc = CD3D11_SAMPLER_DESC(), UINT slot = 0u,
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
		void Bind(ID3D11DeviceContext* context) noexcept override;

	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
		UINT slot;
		Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER;
	};

}
