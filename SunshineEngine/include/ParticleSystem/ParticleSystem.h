#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <SimpleMath.h>

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/unordered_map.h>
#include <EASTL/string.h>

#include <ParticleSystem/Particle.h>

#include <Utils/UUID.h>

namespace DX = DirectX;
namespace DXSM = DirectX::SimpleMath;

class AssetPath;

class TransformComponent;

namespace SE_G
{
	class DeferredRenderer;

	class Camera;

	namespace Bind
	{
		class BlendState;
		class Sampler;
	}
}

namespace SE
{
	class ParticleData;

	class ParticleSystem
	{
	public:

		// Common Shaders
		Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
		Microsoft::WRL::ComPtr<ID3DBlob>				m_vertexShaderBytecodeBlob;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_renderParticleVS;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>    m_renderParticleGS;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_renderParticlePS;

		Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_resetCShader;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_initSimulateDispatchArgsCShader; // pre-simulate
		Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_emitParticlesCShader;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_simulateParticlesCShader;

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthState;
		eastl::unique_ptr<SE_G::Bind::BlendState> m_blendState;
		eastl::unique_ptr<SE_G::Bind::Sampler> m_textureSampler;

		SE_G::DeferredRenderer* m_renderer;
		eastl::shared_ptr<SE_G::Camera> m_camera;


		struct SceneConstantBuffer
		{
			DXSM::Vector4 camPosition;
			float dt;
			float rngSeed;
			UINT padding[2];
		};
		SceneConstantBuffer m_sceneConstantBufferData;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_sceneConstantBuffer;

		struct TransformsParticles
		{
			DX::XMMATRIX viewMat;
			DX::XMMATRIX projMat;
			DX::XMMATRIX viewProjMat;
		};
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_viewProjBuffer;

		bool m_enabled = true;

		ParticleSystem(SE_G::DeferredRenderer* renderer,
			eastl::shared_ptr<SE_G::Camera> camera);
		~ParticleSystem();

		void AddEmitter(SE::UUID uuid, eastl::shared_ptr<ParticleData> particleData);

		void LoadCS(AssetPath shaderPath, Microsoft::WRL::ComPtr<ID3D11ComputeShader>& m_computeShader);

		void ComputePassForAllEmitters();
		void Update(float deltaTime);
		void UpdateAllEmitters(float deltaTime);
		void DisableAllEmitters();
		void EnableAllEmitters();
		void RenderAllEmitters();

		//void Update(float deltaTime);

		void SetBlendState(eastl::unique_ptr<SE_G::Bind::BlendState> newBlendState);

		void SetRenderer(SE_G::DeferredRenderer* renderer);
		void SetCamera(eastl::shared_ptr<SE_G::Camera> camera);

		void Enable();
		void Disable();

		eastl::unordered_map<SE::UUID, eastl::shared_ptr<ParticleData>> m_emitters;
	};
}
