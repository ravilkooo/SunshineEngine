#pragma once



#include "Bindable.h"

namespace Bind
{
    class InputLayout :
        public Bindable
    {
    public:
        InputLayout(ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* inputElements, UINT numInputElements, ID3DBlob* pShaderBytecodeBlob);
        void Bind(ID3D11DeviceContext* context) noexcept override;

    private:
        Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
    };

}
