#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <string>
#include "Scene.h"
#include <Bindable/Bindable.h>
#include <GameObject.h>
#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>

class RenderPass
{
	friend class GameObject;
public:
	RenderPass(eastl::string techniqueTag, ID3D11Device* device, ID3D11DeviceContext* context);
	eastl::string GetTechnique();
	virtual void Pass(const Scene& scene);
	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;

	void AddPerFrameBind(Bind::Bindable* bind);
	void BindAllPerFrame();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

protected:
	eastl::string techniqueTag;
	eastl::vector<Bind::Bindable*> perFrameBindables;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
};
