#pragma once

//#include <IResource.h>

#include "Graphics/Bindable/Bindable.h"
#include <d3dcompiler.h>
#include <EASTL/string.h>

namespace SE_G {
    namespace Bind {
        class PixelShader :
            public Bindable
            // , public Resource
        {
        public:
            PixelShader() {};
            PixelShader(ID3D11Device* device, eastl::wstring filePath) :
                PixelShader(device, filePath.c_str()) {};
            PixelShader(ID3D11Device* device, LPCWSTR filePath);
            ~PixelShader();
            void Release();
            void ChangeShader(ID3D11Device* device, eastl::wstring filePath);
            void ChangeShader(ID3D11Device* device, LPCWSTR filePath);
            void Bind(ID3D11DeviceContext* context) noexcept override;
        private:
            Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
        };
    }
}