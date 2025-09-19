#pragma once



#include "Bindable.h"

namespace Bind
{
	class Rasterizer :
	public Bindable
	{
	public:
		// Set D3D11_CULL_BACK, D3D11_FILL_SOLID
		Rasterizer(ID3D11Device* device,
			D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{}));
		void Bind(ID3D11DeviceContext* context) noexcept override;

	private:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
		// D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	};

}
