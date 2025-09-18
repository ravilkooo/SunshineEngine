

#include "VertexBuffer.h"

Bind::VertexBuffer::VertexBuffer(ID3D11Device* device, const void* data,
    UINT count, UINT stride, VBType type, UINT slot)
    : count(count), slot(slot), type(type)
{

    this->stride = (UINT*)calloc(1, sizeof(UINT));
    this->offset = (UINT*)calloc(1, sizeof(UINT));
    this->stride[0] = stride;
    this->offset[0] = 0;

    D3D11_BUFFER_DESC bd = {};

    if (type == Bind::VertexBuffer::VBType::SINGLE)
    {
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = count * stride;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;
    }
    else if (type == Bind::VertexBuffer::VBType::INSTANCED_CPU)
    {
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = count * stride;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;
    }

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = data;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    device->CreateBuffer(&bd, &InitData, &pVertexBuffer);
}

void Bind::VertexBuffer::Bind(ID3D11DeviceContext* context) noexcept
{
    context->IASetVertexBuffers(slot, 1u, pVertexBuffer.GetAddressOf(), stride, offset);
}

void Bind::VertexBuffer::Update(ID3D11DeviceContext* context, const void* data)
{
    if (type == VBType::INSTANCED_CPU)
    {
        /*
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        context->Map(pVertexBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
        memcpy(mappedResource.pData, data, (count * stride[0]) + (16 - ((count * stride[0]) % 16))); // aligned size
        context->Unmap(pVertexBuffer.Get(), 0);
        */

        D3D11_MAPPED_SUBRESOURCE mappedResource;

        context->Map(pVertexBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
        memcpy(mappedResource.pData, data, (count * stride[0]) + (16 - ((count * stride[0]) % 16))); // aligned size
        context->Unmap(pVertexBuffer.Get(), 0);

    }
}

void Bind::VertexBuffer::Update(ID3D11DeviceContext* context, ID3D11Buffer* inputResource)
{
    if (type == VBType::INSTANCED_CPU)
    {
        /*
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        context->Map(pVertexBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
        memcpy(mappedResource.pData, data, (count * stride[0]) + (16 - ((count * stride[0]) % 16))); // aligned size
        context->Unmap(pVertexBuffer.Get(), 0);
        */

        D3D11_MAPPED_SUBRESOURCE mappedResourceRead;
        D3D11_MAPPED_SUBRESOURCE mappedResourceWrite;

        context->Map(inputResource, 0u, D3D11_MAP_READ, 0u, &mappedResourceRead);
        context->Map(pVertexBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResourceWrite);

        memcpy(mappedResourceWrite.pData, mappedResourceRead.pData, (count * stride[0]) + (16 - ((count * stride[0]) % 16))); // aligned size

        context->Unmap(inputResource, 0);
        context->Unmap(pVertexBuffer.Get(), 0);

    }
}
