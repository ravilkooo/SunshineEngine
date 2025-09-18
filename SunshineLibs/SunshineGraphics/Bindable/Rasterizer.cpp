

#include "Rasterizer.h"

namespace Bind
{
	Rasterizer::Rasterizer(ID3D11Device* device, D3D11_RASTERIZER_DESC rasterDesc)
	{
		device->CreateRasterizerState(&rasterDesc, &pRasterizer);
	}
	void Rasterizer::Bind(ID3D11DeviceContext* context) noexcept
	{
		context->RSSetState(pRasterizer.Get());
	}
}