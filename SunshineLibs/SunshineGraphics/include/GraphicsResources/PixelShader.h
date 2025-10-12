#pragma once

#include <Resource.h>

#include "Bindable/Bindable.h"
#include <d3dcompiler.h>

namespace Bind {
    class PixelShader :
        public Bindable, Resource
    {
    public:
        PixelShader() {};
        PixelShader(ID3D11Device* device, std::string filePath) : PixelShader(device, std::wstring(filePath.begin(), filePath.end()).c_str()) {};
        PixelShader(ID3D11Device* device, LPCWSTR filePath);
        ~PixelShader();
        void Bind(ID3D11DeviceContext* context) noexcept override;
    private:
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
    };
}
