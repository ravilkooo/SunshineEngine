#pragma once



#include "Bindable.h"

namespace Bind
{
    class IndexBuffer :
        public Bindable
    {
    public:
        IndexBuffer(ID3D11Device* device, const void* data, UINT count);
        
        void Bind(ID3D11DeviceContext* context) noexcept override;
        

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
        UINT count;
        // Inherited via Bindable
    };

}
