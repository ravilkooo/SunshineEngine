#pragma once

//#include <IResource.h>

#include <Graphics/Bindable/Bindable.h>
#include <d3dcompiler.h>
#include <EASTL/string.h>
#include <Graphics/GraphicsResources/IShader.h>

namespace SE_G {
    namespace Bind {
        class GeometryShader :
            public Bindable, public IShader
        {
        public:
            GeometryShader() {};
            GeometryShader(ID3D11Device* device, eastl::wstring filePath)
                : GeometryShader(device, filePath.c_str()) {
            };
            GeometryShader(ID3D11Device* device, LPCWSTR filePath);
            ~GeometryShader();

            SE_G::Bind::PipelineStage GetPipelineStage() override { return SE_G::Bind::PipelineStage::GEOMETRY_SHADER; }

            void Release();
            void Bind(ID3D11DeviceContext* context) noexcept override;
        private:
            Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
        };
    }
}
