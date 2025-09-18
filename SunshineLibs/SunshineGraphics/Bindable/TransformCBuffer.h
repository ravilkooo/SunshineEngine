#pragma once



#include <directxmath.h>
#include "Bindable.h"
#include "ConstantBuffer.h"
#include "GraphicsUtils/Drawable.h"

namespace Bind
{
	class TransformCBuffer : public Bindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX wMat;
			DirectX::XMMATRIX wMatInvTranspose;
			DirectX::XMMATRIX viewProj;
		};
	public:
		TransformCBuffer(ID3D11Device* device, Drawable* parent, UINT slot = 0u);
		void Bind(ID3D11DeviceContext* context) noexcept override;
	private:
		// static
		VertexConstantBuffer<Transforms>* pVcbuf;
		Drawable* pParent = nullptr;
	};
}
