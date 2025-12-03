#include <Graphics/Renderer/Technique/LightTechnique.h>

namespace SE_G
{
	bool LightStaticData::s_staticDataInitializated = false;
	eastl::shared_ptr<Bind::DepthStencilState> LightStaticData::depthCompLess;
	eastl::shared_ptr<Bind::DepthStencilState> LightStaticData::depthCompGreater;
	eastl::shared_ptr<Bind::Rasterizer> LightStaticData::rastCullNone;
	eastl::shared_ptr<Bind::Rasterizer> LightStaticData::rastCullBack;
	eastl::shared_ptr<Bind::Rasterizer> LightStaticData::rastCullFront;

	void LightStaticData::InitStaticData(ID3D11Device* device)
	{
		// Depth
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthCompLess = eastl::make_shared<Bind::DepthStencilState>(device, dsDesc);

		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
		depthCompGreater = eastl::make_shared<Bind::DepthStencilState>(device, dsDesc);

		// Rasterizer
		D3D11_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.CullMode = D3D11_CULL_NONE;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rastCullNone = eastl::make_shared<Bind::Rasterizer>(device, rasterDesc);

		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rastCullBack = eastl::make_shared<Bind::Rasterizer>(device, rasterDesc);

		rasterDesc.CullMode = D3D11_CULL_FRONT;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rastCullFront = eastl::make_shared<Bind::Rasterizer>(device, rasterDesc);

		s_staticDataInitializated = true;
	};

}