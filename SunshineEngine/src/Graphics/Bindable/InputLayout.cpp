#include "Graphics/Bindable/InputLayout.h"


namespace SE_G {
    namespace Bind {
        InputLayout::InputLayout(ID3D11Device* device,
            D3D11_INPUT_ELEMENT_DESC* inputElements, UINT numInputElements,
            ID3DBlob* pShaderBytecodeBlob)
        {
            device->CreateInputLayout(
                inputElements,
                numInputElements,
                pShaderBytecodeBlob->GetBufferPointer(),
                pShaderBytecodeBlob->GetBufferSize(),
                &pInputLayout);
        }
        
        InputLayout::~InputLayout() {
            pInputLayout.Reset();
        }

        void InputLayout::Bind(ID3D11DeviceContext* context) noexcept
        {
            context->IASetInputLayout(pInputLayout.Get());
        }

    }
}