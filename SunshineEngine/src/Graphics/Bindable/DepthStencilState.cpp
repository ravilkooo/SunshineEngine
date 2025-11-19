#include "Graphics/Bindable/DepthStencilState.h"

namespace SE_G {
	namespace Bind
	{
		DepthStencilState::DepthStencilState(ID3D11Device* device, D3D11_DEPTH_STENCIL_DESC depthStencilDesc)
		{
			device->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);
		}

		DepthStencilState::~DepthStencilState()
		{
			pDepthStencilState.Reset();
		}

		void DepthStencilState::Bind(ID3D11DeviceContext* context) noexcept
		{
			context->OMSetDepthStencilState(pDepthStencilState.Get(), 1u);
		}


	}
}