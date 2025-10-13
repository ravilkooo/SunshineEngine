#pragma once



#include <directxmath.h>
#include "Bindable.h"
#include "ConstantBuffer.h"
#include "GraphicsUtils/Transformable.h"

namespace Bind
{
	class TransformComponent;

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
		TransformCBuffer(ID3D11Device* device, Transformable* parent, UINT slot = 0u);
		void Bind(ID3D11DeviceContext* context) noexcept override;
	private:
		// static
		VertexConstantBuffer<Transforms>* pVcbuf;
		Transformable* pParent = nullptr;
	};
}
