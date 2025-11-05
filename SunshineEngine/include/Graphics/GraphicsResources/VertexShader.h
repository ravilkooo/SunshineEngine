#pragma once

//#include <IResource.h>

#include <Graphics/Bindable/Bindable.h>
#include <d3dcompiler.h>
#include <EASTL/string.h>

namespace SE_G {
    namespace Bind
    {
        class VertexShader :
            public Bindable
            //, public Resource
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
            void Release();

            void Bind(ID3D11DeviceContext* context) noexcept override;

            ID3DBlob* GetBytecode() const noexcept;

        private:
            Microsoft::WRL::ComPtr<ID3DBlob> pShaderBytecodeBlob;
            Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
            Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
        };
    }
}