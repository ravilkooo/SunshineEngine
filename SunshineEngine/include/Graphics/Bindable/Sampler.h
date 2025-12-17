#pragma once

#include <Graphics/Bindable/Bindable.h>
#include <EASTL/unordered_map.h>
#include <EASTL/string.h>

namespace SE_G {
	namespace Bind
	{
		enum class SamplerPreset {
			Wrap, Mirror, Clamp, Border
		};

		class Sampler :
			public Bindable
		{
		public:
			// Static preset methods
			static D3D11_SAMPLER_DESC WrapDesc();
			static D3D11_SAMPLER_DESC MirrorDesc();
			static D3D11_SAMPLER_DESC ClampDesc();
			static D3D11_SAMPLER_DESC BorderDesc();

			Sampler(ID3D11Device* device,
				D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(), UINT slot = 0u,
				PipelineStage pipelineStage = PipelineStage::PIXEL_SHADER);

			Sampler(ID3D11Device* device,
				SamplerPreset preset, UINT slot = 0u,
				PipelineStage pipelineStage = PipelineStage::PIXEL_SHADER);

			~Sampler();
			
			void Release();
			void ChangePreset(ID3D11Device* device, SamplerPreset preset = SamplerPreset::Wrap);
			SamplerPreset GetPreset();

			void Bind(ID3D11DeviceContext* context) noexcept override;

			PipelineStage m_pipelineStage = PipelineStage::PIXEL_SHADER;

		private:
			SamplerPreset m_preset = SamplerPreset::Wrap;
			Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
			UINT slot;
			bool isNull = true;
		};

	}
}