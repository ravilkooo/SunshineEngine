#pragma once

//#include <IResource.h>

#include "Graphics/Bindable/Bindable.h"
#include <EASTL/string.h>
#include <Graphics/GraphicsResources/IShader.h>

namespace SE_G {
    namespace Bind {
        class PixelShader :
            public Bindable, public IShader
        {
        public:
            PixelShader() {};
            PixelShader(ID3D11Device* device, eastl::wstring filePath) :
                PixelShader(device, filePath.c_str()) {};
            PixelShader(ID3D11Device* device, LPCWSTR filePath);
            ~PixelShader();

            SE_G::Bind::PipelineStage GetPipelineStage() override { return SE_G::Bind::PipelineStage::PIXEL_SHADER; }

            void Release();
            void ChangeShader(ID3D11Device* device, eastl::wstring filePath);
            void ChangeShader(ID3D11Device* device, LPCWSTR filePath);
            void Bind(ID3D11DeviceContext* context) noexcept override;
        private:
            Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
        };
    }
}