#include <Graphics/Renderer/ShadowMap.h>

namespace SE_G
{
	ShadowMap::ShadowMap(ID3D11Device* device, UINT mapWidth, UINT mapHeight)
		: m_mapWidth(mapWidth), m_mapHeight(mapHeight)
	{
		OnResize(device, mapWidth, mapHeight);
		isValid = true;
	}

	ShadowMap::~ShadowMap()
	{
		Release();
	}

	void ShadowMap::Release()
	{
		if (!isValid)
			return;

		m_shadowTexture.Reset();

		for (size_t i = 0; i < 4; i++)
		{
			m_depthDSV[i].Reset();
		}
	}

	void ShadowMap::OnResize(ID3D11Device* device, UINT mapWidth, UINT mapHeight)
	{
		m_mapWidth = mapWidth;
		m_mapHeight = mapHeight;

		Release();

		// Texture for depth values (for shadowing)
		D3D11_TEXTURE2D_DESC depthDesc = {};
		depthDesc.Width = mapWidth;
		depthDesc.Height = mapHeight;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 4;
		depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		depthDesc.MiscFlags = 0;

		device->CreateTexture2D(&depthDesc, nullptr, m_shadowTexture.GetAddressOf());

		// View texture as Depth buffer while rendering from light camera
		D3D11_DEPTH_STENCIL_VIEW_DESC dViewDesc = { };
		dViewDesc.Flags = 0;
		dViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dViewDesc.Texture2DArray.MipSlice = 0;
		dViewDesc.Texture2DArray.ArraySize = 1;

		for (size_t i = 0; i < 4; i++)
		{
			dViewDesc.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, i, 1);
			device->CreateDepthStencilView(m_shadowTexture.Get(), &dViewDesc, m_depthDSV[i].GetAddressOf());
		}
	}
}