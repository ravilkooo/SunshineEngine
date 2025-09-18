#pragma once



#include <d3d11.h>
#include <wrl.h>    
#include <directxmath.h>

#include "Windows/DisplayWindow.h"
#include "GraphicsUtils/Scene.h"
#include "RenderPass.h"

class RenderingSystem
{
public:
	ID3D11Device* GetDevice() { return device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return context.Get(); }

	virtual void RenderScene(const Scene& scene) = 0;

	virtual void SetMainCamera(Camera* camera) { this->mainCamera = camera; }
	virtual Camera* GetMainCamera() { return mainCamera; };

	ID3D11Texture2D* GetBackBuffer() { return backBuffer.Get(); }

	virtual void AddPass(RenderPass* pass) = 0;

	Camera* mainCamera;
protected:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

	D3D_FEATURE_LEVEL featureLevels[1] = { D3D_FEATURE_LEVEL_11_1 };

	DisplayWindow* displayWindow;

	UINT screenWidth = 800;
	UINT screenHeight = 800;

};
