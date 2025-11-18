#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <Graphics/Renderer/Pass/RenderPass.h>

namespace SE_G {
	class RenderGroup {
	public:
		RenderGroup(
			eastl::string name,
			ID3D11Device* device, ID3D11DeviceContext* context);

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

		// == void RenderScene();
		void Pass();
		void AddPass(eastl::shared_ptr<RenderPass> pass);
		void AddTechnique(eastl::unique_ptr<RenderTechnique> tech);

		bool IsEnabled();
		void Disable();
		void Enable();

		ID3D11Device* m_device;
		ID3D11DeviceContext* m_context;

		eastl::string m_groupName;

		eastl::vector<eastl::shared_ptr<RenderPass>> m_passes;
	
	protected:
		bool m_enabled = true;
	};
}