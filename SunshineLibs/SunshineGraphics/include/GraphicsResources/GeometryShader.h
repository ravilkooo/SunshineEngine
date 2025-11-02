#pragma once

#include <Resource.h>

#include "Bindable/Bindable.h"
#include <d3dcompiler.h>
#include <EASTL/string.h>

namespace Bind {
    class GeometryShader :
        public Bindable, Resource
    {
    public:
        GeometryShader() {};
        GeometryShader(ID3D11Device* device, eastl::wstring filePath)
            : GeometryShader(device, std::wstring(filePath.begin(), filePath.end()).c_str()) {};
        GeometryShader(ID3D11Device* device, LPCWSTR filePath);
        ~GeometryShader();
        void Bind(ID3D11DeviceContext* context) noexcept override;
    private:
        Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
    };
}
