#include "IndexBuffer.h"

Bind::IndexBuffer::IndexBuffer(ID3D11Device* device, const void* data, UINT count)
	: count(count)
{
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.ByteWidth = count * sizeof(UINT);

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = data;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	device->CreateBuffer(&bd, &InitData, &pIndexBuffer);
}

void Bind::IndexBuffer::Bind(ID3D11DeviceContext* context) noexcept
{
	context->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}
