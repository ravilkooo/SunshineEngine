#include "Graphics/Bindable/TransformCBuffer.h"
#include <Component/TransformComponent.h>
#include <iostream>

namespace SE_G {
	namespace Bind {

		TransformCBuffer::TransformCBuffer(ID3D11Device* device, TransformComponent* parent, UINT slot)
			: pParent(parent)
		{
			//if (!pVcbuf)
			{
				pVcbuf = eastl::make_unique<VertexConstantBuffer<Transforms>>(device, slot);
			}
		}

		TransformCBuffer::~TransformCBuffer()
		{
			
		}

		void TransformCBuffer::Update(ID3D11DeviceContext* context) noexcept
		{
			const auto wMat = pParent->GetWorldMatrix();
			DXSM::Matrix A = wMat;
			// Correct ?
			A._41 = 0;
			A._42 = 0;
			A._43 = 0;
			A._44 = 1;

			const auto wMatInvTranspose = (A.Invert()).Transpose();

			const Transforms tf = {
				wMat, wMatInvTranspose,
			};
			pVcbuf->Update(context, tf);
		}
		void TransformCBuffer::Bind(ID3D11DeviceContext* context) noexcept
		{
			Update(context);
			pVcbuf->Bind(context);

			/*
			const auto wMat = pParent->worldMat;
			const auto vpMat = pParent->GetViewMatrix() * pParent->GetProjectionMatrix();
			DirectX::XMMATRIX A = wMat;
			A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			DirectX::XMVECTOR det = XMMatrixDeterminant(A);
			const auto wMatInvTranspose = DirectX::XMMatrixTranspose(XMMatrixInverse(&det, A));

			const Transforms tf = {
					wMat, wMatInvTranspose, vpMat
			};
			pVcbuf->Update(context, tf);
			pVcbuf->Bind(context);
			*/
		}

		// VertexConstantBuffer<TransformCBuffer::Transforms>* TransformCBuffer::pVcbuf;
	}
}