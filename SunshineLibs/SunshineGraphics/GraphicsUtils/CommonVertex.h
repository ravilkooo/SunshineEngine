#pragma once



#include <DirectXMath.h>

struct CommonVertex
{
public:
    CommonVertex() {}
    CommonVertex(float x, float y, float z, float r, float g, float b, float a)
        : pos(x, y, z), color(r, g, b, a) {
    }
    CommonVertex(float x, float y, float z, float r, float g, float b)
        : pos(x, y, z), color(r, g, b, 1) {
    }
    CommonVertex(float x, float y, float z)
        : pos(x, y, z), color(0, 0, 0, 1) {
    }
    CommonVertex(DirectX::XMFLOAT3 pos)
        : pos(pos) {
    }
    CommonVertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color)
        : pos(pos), color(color) {
    }
    CommonVertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 normal)
        : pos(pos), color(color), normal(normal) {
    }
    CommonVertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT2 texCoord)
        : pos(pos), color(color), normal(normal), texCoord(texCoord) {
    }
    CommonVertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color, DirectX::XMFLOAT2 texCoord, DirectX::XMFLOAT3 normal)
        : pos(pos), color(color), normal(normal), texCoord(texCoord) {
    }
    CommonVertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 normal)
        : pos(pos), normal(normal) {
    }
    CommonVertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color, DirectX::XMFLOAT2 texCoord)
        : pos(pos), color(color), texCoord(texCoord) {
    }
    CommonVertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT2 texCoord)
        : pos(pos), normal(normal), texCoord(texCoord) {
    }
    CommonVertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 texCoord)
        : pos(pos), texCoord(texCoord) {
    }
    CommonVertex(const CommonVertex& cv)
        : pos(cv.pos), color(cv.color), normal(cv.normal), texCoord(cv.texCoord) {
    }
    /*
    CommonVertex& operator=(const CommonVertex& src) {
        this->pos = src.pos;
        this->color = src.color;
        this->texCoord = src.texCoord;
        this->normal = src.normal;
        return *this;
    }
    */
    DirectX::XMFLOAT3 pos = { 0, 0, 0 };
    DirectX::XMFLOAT4 color = { 0, 0, 0, 1 };
    DirectX::XMFLOAT2 texCoord = { 0, 0 };
    DirectX::XMFLOAT3 normal = { 0, 0, 0 };
};

