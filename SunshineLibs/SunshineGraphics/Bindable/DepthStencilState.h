#pragma once



#include "Bindable.h"

namespace Bind {
    class DepthStencilState :
        public Bindable
    {
    public:
        DepthStencilState(ID3D11Device* device,
            D3D11_DEPTH_STENCIL_DESC depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{}));
        void Bind(ID3D11DeviceContext* context) noexcept override;

    private:
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;
    };
}
