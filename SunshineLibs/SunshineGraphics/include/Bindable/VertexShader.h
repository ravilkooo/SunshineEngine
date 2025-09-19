#pragma once



#include "Bindable.h"
#include <d3dcompiler.h>


namespace Bind
{
    class VertexShader :
        public Bindable
    {
    public:
        VertexShader(ID3D11Device* device, LPCWSTR filePath);
        void Bind(ID3D11DeviceContext* context) noexcept override;
        ID3DBlob* GetBytecode() const noexcept;

    private:
        Microsoft::WRL::ComPtr<ID3DBlob> pShaderBytecodeBlob;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
    };
}
