#pragma once



#include "Bindable.h"

namespace Bind {
    class BlendState :
        public Bindable
    {
    public:
        BlendState(ID3D11Device* device,
            D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{}),
            FLOAT* blendFactor = NULL,
            UINT sampleMask = 0xffffffff);
        void Bind(ID3D11DeviceContext* context) noexcept override;

    private:
        Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
        FLOAT blendFactor[4] = { 1, 1, 1, 1 };
        UINT sampleMask = 0xffffffff;
    };
}
