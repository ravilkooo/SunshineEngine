//#pragma once
//#include <cstdint>
//#include <ResourceManager/ResourceHandle.h>
//#include <dxgiformat.h>
//#include <d3d11.h>
//#include <ResourceManager/Enums/ResourceType.h>
//#include <ResourceManager/IResource.h>
//
//class Texture : public IResource
//{
//public:
//    ResourceType GetType() const override { return ResourceType::TEXTURE; }
//    ResourceGUID GetGUID() const override { return m_GUID; }
//    size_t GetSizeInMemory() const override { return m_MemorySize; }
//    bool PostLoadInit() override;
//
//    uint8_t* GetData() { return m_pData; }
//    uint32_t GetWidth() const { return m_Width; }
//    uint32_t GetHeight() const { return m_Height; }
//    DXGI_FORMAT GetFormat() const { return m_Format; }
//
//    // DirectX ресурсы (созданы в PostLoadInit)
//    ID3D11ShaderResourceView* GetSRV() const { return m_pSRV; }
//
//private:
//    ResourceGUID m_GUID;
//    uint8_t* m_pData = nullptr;
//    size_t m_MemorySize = 0;
//    uint32_t m_Width = 0;
//    uint32_t m_Height = 0;
//    DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
//
//    // DirectX ресурсы
//    ID3D11Texture2D* m_pTexture2D = nullptr;
//    ID3D11ShaderResourceView* m_pSRV = nullptr;
//
//    friend class TextureLoader;
//};