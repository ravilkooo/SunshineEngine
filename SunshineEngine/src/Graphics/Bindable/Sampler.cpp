#include <Graphics/Bindable/Sampler.h>


namespace SE_G {
	namespace Bind
	{
		Sampler::Sampler(ID3D11Device* device, D3D11_SAMPLER_DESC samplerDesc,
			UINT slot, PipelineStage pipelineStage)
			: slot(slot), m_pipelineStage(pipelineStage), m_preset(SamplerPreset::Wrap)
		{
			device->CreateSamplerState(&samplerDesc, &pSampler);
			isNull = false;
		}


		Sampler::Sampler(ID3D11Device* device,
			SamplerPreset preset, UINT slot,
			PipelineStage pipelineStage)
			: slot(slot), m_pipelineStage(pipelineStage), m_preset(preset) {
			ChangeSampler(device, preset);
		}

		Sampler::~Sampler() {
			Release();
		}

		void Sampler::Release() {
			if (!isNull)
				pSampler.ReleaseAndGetAddressOf();
		}

		void Sampler::ChangeSampler(ID3D11Device* device, SamplerPreset preset) {
			Release();
			D3D11_SAMPLER_DESC samplerDesc;
			switch (preset)
			{
			case SamplerPreset::Wrap:
				 samplerDesc = WrapDesc();
				device->CreateSamplerState(&samplerDesc, &pSampler);
				break;
			case SamplerPreset::Mirror:
				samplerDesc = MirrorDesc();
				device->CreateSamplerState(&samplerDesc, &pSampler);
				break;
			case SamplerPreset::Clamp:
				samplerDesc = ClampDesc();
				device->CreateSamplerState(&samplerDesc, &pSampler);
				break;
			case SamplerPreset::Border:
				samplerDesc = BorderDesc();
				device->CreateSamplerState(&samplerDesc, &pSampler);
				break;
			default:
				m_preset = SamplerPreset::Wrap;
				samplerDesc = WrapDesc();
				device->CreateSamplerState(&samplerDesc, &pSampler);
				break;
			}
		}

		void Sampler::Bind(ID3D11DeviceContext* context) noexcept
		{
			if (m_pipelineStage == PipelineStage::PIXEL_SHADER)
				context->PSSetSamplers(slot, 1u, pSampler.GetAddressOf());
			else if (m_pipelineStage == PipelineStage::COMPUTE_SHADER)
				context->CSSetSamplers(slot, 1u, pSampler.GetAddressOf());
		}


		// Preset for D3D11_TEXTURE_ADDRESS_WRAP
		D3D11_SAMPLER_DESC Sampler::WrapDesc()
		{
			auto s =  D3D11_SAMPLER_DESC(
				CD3D11_SAMPLER_DESC()
			);
			
			s.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			s.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			s.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

			return s;
		}

		// Preset for D3D11_TEXTURE_ADDRESS_MIRROR
		D3D11_SAMPLER_DESC Sampler::MirrorDesc()
		{
			auto s = D3D11_SAMPLER_DESC(
				CD3D11_SAMPLER_DESC()
			);

			s.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
			s.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
			s.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;

			return s;
		}

		// Preset for D3D11_TEXTURE_ADDRESS_CLAMP
		D3D11_SAMPLER_DESC Sampler::ClampDesc()
		{
			auto s = D3D11_SAMPLER_DESC(
				CD3D11_SAMPLER_DESC()
			);

			s.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			s.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			s.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

			return s;
		}

		// Preset for D3D11_TEXTURE_ADDRESS_BORDER
		D3D11_SAMPLER_DESC Sampler::BorderDesc()
		{
			auto s = D3D11_SAMPLER_DESC(
				CD3D11_SAMPLER_DESC()
			);

			s.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			s.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			s.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

			return s;
		}
	}
}