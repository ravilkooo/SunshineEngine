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
		virtual ~RenderGroup();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

		// == void RenderScene();
		virtual void Pass();
		virtual RenderPass* AddPass(eastl::unique_ptr<RenderPass> pass);
		virtual RenderPass* GetPass(RenderPass::PassType passType);
		virtual void RemovePass(RenderPass::PassType passType);

		RenderTechnique* AddTechnique(SE::UUID uuid, eastl::unique_ptr<RenderTechnique> tech);
		RenderTechnique* GetTechnique(SE::UUID uuid, eastl::string techniqueTag);
		void RemoveTechnique(SE::UUID uuid, eastl::string techniqueTag);
		void RemoveAllTechniques(SE::UUID uuid);
		void ClearAllTechniques();

		bool IsEnabled();
		void Disable();
		void Enable();

		ID3D11Device* m_device;
		ID3D11DeviceContext* m_context;

		eastl::string m_groupName;

		eastl::unordered_map<RenderPass::PassType, eastl::unique_ptr<RenderPass>> m_passes;;
	
	protected:
		bool m_enabled = true;
	};
}