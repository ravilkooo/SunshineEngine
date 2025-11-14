#pragma once

#include "Bindable.h"

namespace SE_G {
    namespace Bind
    {
        class IndexBuffer :
            public Bindable
        {
        public:
            IndexBuffer(ID3D11Device* device, const void* data, UINT count);
            ~IndexBuffer();

            void Release();

            void Bind(ID3D11DeviceContext* context) noexcept override;


        private:
            bool isNull = true;
            Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
            UINT count;
            // Inherited via Bindable
        };

    }
}
