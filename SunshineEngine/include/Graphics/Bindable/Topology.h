#pragma once

#include "Bindable.h"

namespace SE_G {
    namespace Bind
    {
        class Topology :
            public Bindable
        {
        public:
            Topology(D3D11_PRIMITIVE_TOPOLOGY type);

            void Bind(ID3D11DeviceContext* context) noexcept override;

        private:
            D3D11_PRIMITIVE_TOPOLOGY type;
        };
    }
}