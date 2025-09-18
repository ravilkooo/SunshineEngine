#pragma once



#include <wrl.h>
#include <d3d11.h>
#include <directxmath.h>
#include <SimpleMath.h>
#include <map>

#include "GraphicsUtils/Camera.h"
#include "CommonVertex.h"
#include "RenderingSystem/RenderTechnique.h"

#include <assimp/scene.h>

namespace Bind {
    class Bindable;
}

class Drawable {
public:
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

    DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMMATRIX GetProjectionMatrix() const;

	DirectX::XMFLOAT3 GetCameraPosition() const;

	virtual void DrawTechnique(std::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const noexcept;

	CommonVertex* vertices;
	UINT verticesNum;

	int* indices;
	UINT indicesNum;

    virtual void Update(float deltaTime) = 0;

    virtual Vector3 GetCenterLocation() = 0;

    D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements;
    UINT numInputElements = 2;

    Camera* camera;

    // void AddChild(SceneNode* child) { children.push_back(child); }
    // const std::vector<SceneNode*>& GetChildren() const { return children; }

    ID3D11Device* device;

    std::map<std::string, RenderTechnique*> techniques;

    bool HasTechnique(std::string technique);
    
    void PassTechnique(std::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
};