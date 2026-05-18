#pragma once


#include <Graphics/Bindable/Bindable.h>
#include <d3dcompiler.h>
#include <EASTL/string.h>
#include <ResourceManager/IResource.h>
#include <Graphics/GraphicsResources/IShader.h>

namespace SE_G {
    namespace Bind
    {
        class VertexShader :
            public Bindable, public IShader
        {
        public:
            VertexShader() {};
            VertexShader(ID3D11Device* device, eastl::wstring filePath)
                : VertexShader(device, filePath.c_str()) {};
            VertexShader(ID3D11Device* device, LPCWSTR filePath);
            VertexShader(ID3D11Device* device, eastl::wstring filePath,
                UINT numInputElements,
                D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements)
                : VertexShader(device,
                    filePath.c_str(),
                    numInputElements,
                    IALayoutInputElements) {
            };
            VertexShader(ID3D11Device* device, LPCWSTR filePath,
                UINT numInputElements,
                D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements);
            ~VertexShader();

            SE_G::Bind::PipelineStage GetPipelineStage() override { return SE_G::Bind::PipelineStage::VERTEX_SHADER; }

            void Release();

            void Bind(ID3D11DeviceContext* context) noexcept override;

            ID3DBlob* GetBytecode() const noexcept;

            // Inherited via IResource
            SunshineResource::ResourceType GetType() const override;
            ResourceGUID GetGUID() const override;
            size_t GetSizeInMemory() const override;
        private:
            ResourceGUID m_GUID = 0;
            size_t m_MemorySize = 0;

            Microsoft::WRL::ComPtr<ID3DBlob> pShaderBytecodeBlob;
            Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
            Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;

        };
    }
}