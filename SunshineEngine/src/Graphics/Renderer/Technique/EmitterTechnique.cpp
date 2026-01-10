#include "Graphics/Renderer/Technique/EmitterTechnique.h"
#include <Graphics/Renderer/Pass/EmitterDebugPass.h>
#include <Graphics/GraphicsResources/VertexShader.h>

#include <ParticleSystem/ParticleEmitter.h>

#include <Utils/StringUtils.h>

namespace SE_G {
	eastl::unique_ptr<Bind::VertexShader> EmitterTechnique::s_pointEmitterShader;

	bool EmitterTechnique::s_staticDataInitializated = false;

	EmitterTechnique::EmitterTechnique(ID3D11Device* device, TransformComponent* assignedTransform,
		eastl::string technique, SE::ParticleData* particleData)
		: RenderTechnique(device, assignedTransform, technique),
		m_emitterData({ {}, SE::EmitterType::Point })
	{
		if (!s_staticDataInitializated) {
			EmitterTechnique::InitStaticData(device);
		}
		m_particleData = particleData;

		m_emitterData.settings.data.asPoint = SE::PointEmitterSettings{
			m_particleData->m_emitterConstantBufferData.longitudeMin,
			m_particleData->m_emitterConstantBufferData.longitudeMax,
			m_particleData->m_emitterConstantBufferData.latitudeMin,
			m_particleData->m_emitterConstantBufferData.latitudeMax,
		};

		m_settingsCB = eastl::make_unique<Bind::VertexConstantBuffer<SE::EmitterSettings>>(
			device,
			m_emitterData.settings,
			2u);
	}

	void EmitterTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		BindAll(context);
		DrawTechnique(context);
	}

	void EmitterTechnique::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		UpdateSettingsCB(context.Get());

		m_settingsCB->Bind(context.Get());


		switch (m_emitterData.emitterType)
		{
		case SE::EmitterType::Point:
			s_pointEmitterShader->Bind(context.Get());
			break;

		default:
			s_pointEmitterShader->Bind(context.Get());
			break;
		}
	}

	void EmitterTechnique::DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		switch (m_emitterData.emitterType)
		{
		case SE::EmitterType::Point:

			context->DrawIndexedInstanced(
				2u,
				9u*17u,
				0u,
				0u,
				0u
			);
			break;

		default:
			break;
		}
	}

	void EmitterTechnique::UpdateSettingsCB(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		switch (m_emitterData.emitterType)
		{
		case SE::EmitterType::Point:
			m_emitterData.settings.data.asPoint = SE::PointEmitterSettings{
				m_particleData->m_emitterConstantBufferData.longitudeMin,
				m_particleData->m_emitterConstantBufferData.longitudeMax,
				m_particleData->m_emitterConstantBufferData.latitudeMin,
				m_particleData->m_emitterConstantBufferData.latitudeMax,
			};
			break;

		default:
			break;
		}
		m_settingsCB->Update(context.Get(), m_emitterData.settings);
	}

	void EmitterTechnique::InitStaticData(ID3D11Device* device)
	{
		UINT numInputElements = 1;
		D3D11_INPUT_ELEMENT_DESC IALayoutInputElements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		s_pointEmitterShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/EmitterDebugPass/PointEmitterVS.hlsl").c_str(),
			numInputElements,
			IALayoutInputElements);
		s_staticDataInitializated = true;
	}
}