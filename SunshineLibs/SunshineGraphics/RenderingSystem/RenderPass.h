#pragma once



#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <GraphicsUtils/Scene.h>


class RenderPass
{
public:
	RenderPass(std::string techniqueTag, ID3D11Device* device, ID3D11DeviceContext* context);
	std::string GetTechnique();
	virtual void Pass(const Scene& scene);
	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;

	void AddPerFrameBind(Bind::Bindable* bind);
	void BindAllPerFrame();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
protected:
	std::string techniqueTag;
	std::vector<Bind::Bindable*> perFrameBindables;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
};

