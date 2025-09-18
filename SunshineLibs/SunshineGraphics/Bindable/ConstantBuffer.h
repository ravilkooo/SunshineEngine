#pragma once



#include "Bindable.h"

namespace Bind
{
	template <class C>
	class ConstantBuffer : public Bindable {
	protected:
		ID3D11Buffer* pConstantBuffer;
		UINT slot;
	public:
		virtual void Update(ID3D11DeviceContext* context, const C& consts) {
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			context->Map(pConstantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
			memcpy(mappedResource.pData, &consts, sizeof(consts) + (16 - (sizeof(consts) % 16))); // aligned size
			context->Unmap(pConstantBuffer, 0);
			//context->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);
		}

		ConstantBuffer(ID3D11Device* device, const C& consts, UINT slot = 0u)
			:
			slot(slot)
		{
			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(consts) + (16 - (sizeof(consts) % 16));  // aligned size
			cbd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA InitData = {};
			InitData.pSysMem = &consts;
			device->CreateBuffer(&cbd, &InitData, &pConstantBuffer);
		}

		ConstantBuffer(ID3D11Device* device, UINT slot = 0u)
			:
			slot(slot)
		{
			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(C) + (16 - (sizeof(C) % 16)); // aligned size
			cbd.StructureByteStride = 0u;
			device->CreateBuffer(&cbd, nullptr, &pConstantBuffer);
		}
	};

	template<typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using ConstantBuffer<C>::slot;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(ID3D11DeviceContext* context) noexcept override
		{
			context->VSSetConstantBuffers(slot, 1u, &pConstantBuffer);
		}
		void Update(ID3D11DeviceContext* context, const C& consts) override {
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			context->Map(pConstantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
			memcpy(mappedResource.pData, &consts, sizeof(consts) + (16 - (sizeof(consts) % 16))); // aligned size
			context->Unmap(pConstantBuffer, 0);
			//context->VSSetConstantBuffers(slot, 1u, &pConstantBuffer);
		}
	};

	template<typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using ConstantBuffer<C>::slot;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(ID3D11DeviceContext* context) noexcept override
		{
			context->PSSetConstantBuffers(slot, 1u, &pConstantBuffer);
		}

		void Update(ID3D11DeviceContext* context, const C& consts) override {
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			context->Map(pConstantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
			memcpy(mappedResource.pData, &consts, sizeof(consts) + (16 - (sizeof(consts) % 16))); // aligned size
			context->Unmap(pConstantBuffer, 0);
			//context->PSSetConstantBuffers(slot, 1u, &pConstantBuffer);
		}
	};
	
	template<typename C>
	class ComputeConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using ConstantBuffer<C>::slot;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(ID3D11DeviceContext* context) noexcept override
		{
			context->CSSetConstantBuffers(slot, 1u, &pConstantBuffer);
		}

		void Update(ID3D11DeviceContext* context, const C& consts) override {
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			context->Map(pConstantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
			memcpy(mappedResource.pData, &consts, sizeof(consts) + (16 - (sizeof(consts) % 16))); // aligned size
			context->Unmap(pConstantBuffer, 0);
			//context->PSSetConstantBuffers(slot, 1u, &pConstantBuffer);
		}
	};
}