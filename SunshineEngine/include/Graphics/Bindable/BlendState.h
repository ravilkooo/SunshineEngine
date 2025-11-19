#pragma once

#include <Graphics/Bindable/Bindable.h>
#include <EASTL/array.h>

namespace SE_G {
    namespace Bind {
        class BlendState :
            public Bindable
        {
        public:
            BlendState(ID3D11Device* device,
                D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{}),
                eastl::array<FLOAT, 4> blendFactor = eastl::array<FLOAT, 4>{ 1.0f, 1.0f, 1.0f, 1.0f },
                UINT sampleMask = 0xffffffff);
            ~BlendState();
            void Bind(ID3D11DeviceContext* context) noexcept override;

        private:
            Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
            eastl::array<FLOAT, 4> m_blendFactor = { 1.0f, 1.0f, 1.0f, 1.0f };
            UINT sampleMask = 0xffffffff;
        };
    }
}