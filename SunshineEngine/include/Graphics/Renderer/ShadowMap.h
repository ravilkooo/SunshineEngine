#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <wrl.h>

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>


namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

namespace SE_G
{
	class ShadowMap
	{
	public:

		ShadowMap(ID3D11Device* device, UINT mapWidth, UINT mapHeight);
		~ShadowMap();

		void Release();

		void OnResize(ID3D11Device* device, UINT mapWidth, UINT mapHeight);

		ID3D11Texture2D* GetTexture() { return m_shadowTexture.Get(); };

		UINT m_mapWidth = 800u;
		UINT m_mapHeight = 800u;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_shadowTexture;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthDSV[4];
	private:
		bool isValid = false;
	};
}