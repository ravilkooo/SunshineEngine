#pragma once

#include <directxmath.h>
#include "Bindable.h"
#include "ConstantBuffer.h"
#include <EASTL/unique_ptr.h>

class TransformComponent;

namespace SE_G {
	namespace Bind
	{

		class TransformCBuffer : public Bindable
		{
		protected:
			struct Transforms
			{
				DirectX::XMMATRIX wMat;
				DirectX::XMMATRIX wMatInvTranspose;
				//DirectX::XMMATRIX viewProj;
			};
		public:
			TransformCBuffer(ID3D11Device* device, TransformComponent* parent, UINT slot = 0u);
			~TransformCBuffer();
			void Bind(ID3D11DeviceContext* context) noexcept override;
		private:
			// static
			eastl::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
			TransformComponent* pParent = nullptr;
		};
	}
}