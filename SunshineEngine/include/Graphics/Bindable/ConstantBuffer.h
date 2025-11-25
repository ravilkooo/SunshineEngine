#pragma once

#include <stdexcept>
#include "Bindable.h"
#include <EASTL/vector.h>
#include <EASTL/memory.h>

template<class C, class T>
concept FitsInC = (sizeof(T) <= sizeof(C));

namespace SE_G {
	namespace Bind
	{
		static constexpr UINT Align16(UINT s) { return (s + 15u) & ~15u; }

		template <class C>
		class ConstantBuffer : public Bindable {
		protected:
			Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
			UINT slot;
		public:
			virtual void Update(ID3D11DeviceContext* context, const C& consts)
			{
				if (!pConstantBuffer) return;

				const UINT aligned = Align16(static_cast<UINT>(sizeof(C)));

				D3D11_MAPPED_SUBRESOURCE mappedResource{};
				HRESULT hr = context->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
				if (FAILED(hr)) return;

				eastl::vector<char> tmp(aligned);
				std::memset(tmp.data(), 0, aligned);
				std::memcpy(tmp.data(), &consts, sizeof(C));

				std::memcpy(mappedResource.pData, tmp.data(), aligned);
				context->Unmap(pConstantBuffer.Get(), 0);
			}

			template <FitsInC<C> T>
			void UpdateWithOtherType(ID3D11DeviceContext* context, const T& consts)
			{
				if (!pConstantBuffer) return;

				const UINT alignedOrigin = Align16(static_cast<UINT>(sizeof(C)));
				const UINT alignedIncoming = Align16(static_cast<UINT>(sizeof(T)));
				const UINT alignedMin = min(alignedIncoming, alignedOrigin);
				const UINT copySize = min(alignedMin, sizeof(T));

				D3D11_MAPPED_SUBRESOURCE mappedResource{};
				HRESULT hr = context->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
				if (FAILED(hr)) return;

				eastl::vector<char> tmp(alignedOrigin);
				std::memset(tmp.data(), 0, alignedOrigin);
				std::memcpy(tmp.data(), &consts, copySize);

				std::memcpy(mappedResource.pData, tmp.data(), alignedOrigin);
				context->Unmap(pConstantBuffer.Get(), 0);
			}

			ConstantBuffer(ID3D11Device* device, const C& consts, UINT slot = 0u)
				:
				slot(slot)
			{
				const UINT aligned = Align16(static_cast<UINT>(sizeof(C)));

				D3D11_BUFFER_DESC cbd{};
				cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				cbd.Usage = D3D11_USAGE_DYNAMIC;
				cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				cbd.MiscFlags = 0u;
				cbd.ByteWidth = aligned;  // aligned size
				cbd.StructureByteStride = 0u;

				D3D11_SUBRESOURCE_DATA InitData{};
				eastl::vector<char> initBuf(aligned);
				std::memset(initBuf.data(), 0, aligned);
				std::memcpy(initBuf.data(), &consts, sizeof(C));
				InitData.pSysMem = initBuf.data();

				HRESULT hr = device->CreateBuffer(&cbd, &InitData, pConstantBuffer.GetAddressOf());
				if (FAILED(hr)) {
					pConstantBuffer.Reset();
					printf("Failed to create ConstantBuffer\n");
					// to-do: change to assert and add logging
					throw std::runtime_error("Failed to create ConstantBuffer");
				}
			}

			ConstantBuffer(ID3D11Device* device, UINT slot = 0u)
				:
				slot(slot)
			{
				const UINT aligned = Align16(static_cast<UINT>(sizeof(C)));

				D3D11_BUFFER_DESC cbd{};
				cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				cbd.Usage = D3D11_USAGE_DYNAMIC;
				cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				cbd.MiscFlags = 0u;
				cbd.ByteWidth = aligned; // aligned size
				cbd.StructureByteStride = 0u;

				HRESULT hr = device->CreateBuffer(&cbd, nullptr, pConstantBuffer.GetAddressOf());
				if (FAILED(hr)) {
					pConstantBuffer.Reset();
					printf("Failed to create ConstantBuffer\n");
					// to-do: change to assert and add logging
					throw std::runtime_error("Failed to create ConstantBuffer");
				}
			}

			virtual ~ConstantBuffer()
			{
				pConstantBuffer.Reset();
			}
		};

		template<typename C>
		class GeometryConstantBuffer : public ConstantBuffer<C>
		{
			using ConstantBuffer<C>::pConstantBuffer;
			using ConstantBuffer<C>::slot;
		public:
			using ConstantBuffer<C>::ConstantBuffer;
			void Bind(ID3D11DeviceContext* context) noexcept override
			{
				context->GSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
			}
			void Update(ID3D11DeviceContext* context, const C& consts) override {
				ConstantBuffer<C>::Update(context, consts);
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
				context->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
			}
			void Update(ID3D11DeviceContext* context, const C& consts) override {
				ConstantBuffer<C>::Update(context, consts);
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
				context->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
			}

			void Update(ID3D11DeviceContext* context, const C& consts) override {
				ConstantBuffer<C>::Update(context, consts);
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
				context->CSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
			}

			void Update(ID3D11DeviceContext* context, const C& consts) override {
				ConstantBuffer<C>::Update(context, consts);
			}
		};
	}
}