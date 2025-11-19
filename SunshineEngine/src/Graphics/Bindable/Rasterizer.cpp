#include "Graphics/Bindable/Rasterizer.h"

namespace SE_G {
	namespace Bind
	{
		Rasterizer::Rasterizer(ID3D11Device* device, D3D11_RASTERIZER_DESC rasterDesc)
		{
			device->CreateRasterizerState(&rasterDesc, &pRasterizer);
		}
		Rasterizer::~Rasterizer()
		{
			pRasterizer.Reset();
		}
		void Rasterizer::Bind(ID3D11DeviceContext* context) noexcept
		{
			context->RSSetState(pRasterizer.Get());
		}
	}
}