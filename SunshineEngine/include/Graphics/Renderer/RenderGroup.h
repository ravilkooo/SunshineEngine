#pragma once
#include <Windows.h> // UINT

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>
#include <EASTL/unordered_map.h>

#include <Utils/UUID.h>

class ID3D11Device;
class ID3D11DeviceContext;

namespace SE_G {
	class RenderTechnique;
	class RenderPass;
	class Camera;

	enum class PassType : unsigned int;

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
		virtual RenderPass* GetPass(PassType passType);
		virtual void RemovePass(PassType passType);

		RenderTechnique* AddTechnique(SE::UUID uuid, eastl::unique_ptr<RenderTechnique> tech);
		RenderTechnique* GetTechnique(SE::UUID uuid, eastl::string techniqueTag);
		void RemoveTechnique(SE::UUID uuid, eastl::string techniqueTag);
		void RemoveAllTechniques(SE::UUID uuid);
		void DisableAllTechniques(SE::UUID uuid);
		void EnableAllTechniques(SE::UUID uuid);
		void ClearAllTechniques();

		bool IsEnabled();
		void Disable();
		void Enable();

		virtual eastl::shared_ptr<Camera> GetMainCamera();

		ID3D11Device* m_device;
		ID3D11DeviceContext* m_context;

		eastl::string m_groupName;

		eastl::unordered_map<PassType, eastl::unique_ptr<RenderPass>> m_passes;
	
	protected:
		bool m_enabled = true;
	};
}