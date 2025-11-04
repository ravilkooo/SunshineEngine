//#include <ResourceManager/Resources/Texture.h>
//
//bool Texture::PostLoadInit()
//{
//	// На этом этапе m_pData содержит пиксельные данные из файла
//	// Нужно создать ID3D11Texture2D и ID3D11ShaderResourceView
//
//	D3D11_TEXTURE2D_DESC desc = {};
//	desc.Width = m_Width;
//	desc.Height = m_Height;
//	desc.MipLevels = 1;
//	desc.ArraySize = 1;
//	desc.Format = m_Format;
//	desc.SampleDesc.Count = 1;
//	desc.Usage = D3D11_USAGE_DEFAULT;
//	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//
//	D3D11_SUBRESOURCE_DATA initData = {};
//	initData.pSysMem = m_pData;
//	initData.SysMemPitch = m_Width * 4;  // 4 байта на пиксель (RGBA)
//
//	HRESULT hr = m_pDevice->CreateTexture2D(&desc, &initData, &m_pTexture2D);
//	if (FAILED(hr))
//		return false;
//
//	// Создать SRV для использования в шейдерах
//	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//	srvDesc.Format = desc.Format;
//	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MipLevels = 1;
//
//	hr = m_pDevice->CreateShaderResourceView(m_pTexture2D, &srvDesc, &m_pSRV);
//	if (FAILED(hr))
//	{
//		m_pTexture2D->Release();
//		return false;
//	}
//
//	// Теперь можно освободить сырые данные (опционально)
//	// delete[] m_pData;
//	// m_pData = nullptr;
//
//	return true;
//}