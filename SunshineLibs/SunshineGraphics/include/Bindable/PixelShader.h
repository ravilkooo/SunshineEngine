#pragma once



#include "Bindable.h"
#include <d3dcompiler.h>

namespace Bind {
    class PixelShader :
        public Bindable
    {
    public:
        PixelShader(ID3D11Device* device, LPCWSTR filePath);
        void Bind(ID3D11DeviceContext* context) noexcept override;
    private:
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
    };
}
