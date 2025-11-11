#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <string>
#include "Scene.h"
#include <Graphics/Bindable/Bindable.h>
#include <GameObject.h>
#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>

#include <Graphics/Renderer/Technique/RenderTechnique.h>

namespace SE_G {
	class RenderPass
	{
		friend class GameObject;
	public:
		RenderPass(eastl::string techniqueTag, ID3D11Device* device, ID3D11DeviceContext* context);
		eastl::string GetTechniqueTag();
		virtual void Pass();
		virtual void StartFrame() = 0;
		virtual void EndFrame() = 0;

		void AddTechnique(eastl::unique_ptr<RenderTechnique> tech);

		void AddPerFrameBind(Bind::Bindable* bind);
		void BindAllPerFrame();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();
		bool IsEnabled();
		void Disable();
		void Enable();

	protected:
		eastl::string techniqueTag;
		eastl::vector<Bind::Bindable*> perFrameBindables;

		Microsoft::WRL::ComPtr<ID3D11Device> device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

		eastl::vector<eastl::unique_ptr<SE_G::RenderTechnique>> m_techniques;

	private:
		bool m_enabled = true;
	};
}