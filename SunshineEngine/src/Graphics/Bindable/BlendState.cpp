#include "Graphics/Bindable/BlendState.h"
#include <stdexcept>

namespace SE_G {
	namespace Bind
	{
		BlendState::BlendState(ID3D11Device* device,
			D3D11_BLEND_DESC blendDesc,
			eastl::array<FLOAT, 4> blendFactor, UINT sampleMask) :
			sampleMask(sampleMask)
		{
			HRESULT hr = device->CreateBlendState(&blendDesc, pBlendState.GetAddressOf());
			if (FAILED(hr)) {
				printf("Failed to create BlendState\n");
				// to-do: change to assert and add logging
				throw std::runtime_error("CreateBlendState failed");
			}

			this->m_blendFactor[0] = blendFactor[0];
			this->m_blendFactor[1] = blendFactor[1];
			this->m_blendFactor[2] = blendFactor[2];
			this->m_blendFactor[3] = blendFactor[3];
		}

		BlendState::~BlendState()
		{
			pBlendState.Reset();
			//delete[] blendFactor;
		}

		void BlendState::Bind(ID3D11DeviceContext* context) noexcept
		{
			context->OMSetBlendState(pBlendState.Get(), m_blendFactor.data(), sampleMask);

		}

	}
}