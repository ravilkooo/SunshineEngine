#pragma once



#include "Bindable.h"
#include <d3dcompiler.h>

namespace Bind
{
    class VertexBuffer :
        public Bindable
    {
    public:
        enum class VBType {
            SINGLE, INSTANCED_CPU
        };

        VertexBuffer(ID3D11Device* device, const void* data, UINT count, UINT stride, VBType type = VBType::SINGLE, UINT slot = 0u);
        
        void Bind(ID3D11DeviceContext* context) noexcept override;

        void Update(ID3D11DeviceContext* context, const void* data);
        void Update(ID3D11DeviceContext* context, ID3D11Buffer* inputResource);

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
        UINT count;
        UINT slot;
        UINT* stride;
        UINT* offset;

        VBType type = VBType::SINGLE;
    };

}
