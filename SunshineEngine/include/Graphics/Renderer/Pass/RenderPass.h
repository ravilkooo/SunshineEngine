#pragma once

#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <d3d11.h>
#include <wrl.h>

#include <Utils/UUID.h>

namespace SE_G {
	class RenderTechnique;
	class Camera;

	namespace Bind {
		class Bindable;
	}

	enum class PassType : UINT {
		GPass,
		Shadow,
		Light,
		Collider,
		Trigger,
		Emitter,
		Icon,
		Selection,
		Perception,
		Count
	};

	class RenderPass
	{
		// friend class GameObject;

	public:

		RenderPass(eastl::string techniqueTag, ID3D11Device* device, ID3D11DeviceContext* context);
		virtual ~RenderPass();

		virtual void ClearTechniques();

		eastl::string GetTechniqueTag();
		virtual void Pass();
		virtual void StartFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void OnResize(UINT resizeWidth, UINT resizeHeight) {};

		RenderTechnique* AddTechnique(SE::UUID uuid, eastl::unique_ptr<RenderTechnique> tech);
		RenderTechnique* GetTechnique(SE::UUID uuid);
		void RemoveTechnique(SE::UUID uuid);

		void AddPerFrameBind(Bind::Bindable* bind);
		void BindAllPerFrame();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();
		bool IsEnabled();
		void Disable();
		void Enable();

		PassType m_passType;

		eastl::shared_ptr<Camera> GetCamera();
		virtual void SetCamera(eastl::shared_ptr<Camera> camera);
		eastl::shared_ptr<Camera> m_camera;

	protected:
		eastl::string techniqueTag;
		eastl::vector<Bind::Bindable*> perFrameBindables;

		Microsoft::WRL::ComPtr<ID3D11Device> device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

		// ������� ���������. ����� ����� ������ �������� �� UUID
		eastl::unordered_map<SE::UUID, eastl::unique_ptr<SE_G::RenderTechnique>> m_techniques;


		bool m_enabled = true;
	};
}