#include "Graphics/Renderer/Technique/EmitterTechnique.h"
#include <Graphics/Renderer/Pass/EmitterDebugPass.h>

#include <Graphics/Bindable/ConstantBuffer.h>
#include <Graphics/GraphicsResources/VertexShader.h>

#include <ParticleSystem/ParticleEmitterComponent.h>

#include <ResourceManager/ResourceManagerFacade.h>

#include <Utils/StringUtils.h>

namespace SE_G {
	eastl::shared_ptr<Bind::VertexShader> EmitterTechnique::s_pointEmitterShader;

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
		AssetPath shaderPath = AssetPath(L"Shaders/EmitterDebugPass/PointEmitterVS.hlsl", AssetPath::AssetSource::Engine);
		shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::VERTEX_SHADER;
		shaderPath.m_params.asShader.numInputElements = 1;
		shaderPath.m_params.asShader.IALayoutInputElements = new D3D11_INPUT_ELEMENT_DESC[shaderPath.m_params.asShader.numInputElements];
		shaderPath.m_params.asShader.IALayoutInputElements[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		auto& rm = ResourceManagerFacade::Instance();
		ResourceHandle iconVshaderHandle = rm.LoadByPath(shaderPath);
		SE_G::Bind::VertexShader* iconVshaderRes = rm.Get<SE_G::Bind::VertexShader>(iconVshaderHandle);
		s_pointEmitterShader = eastl::shared_ptr<SE_G::Bind::VertexShader>(
			iconVshaderRes,
			[](SE_G::Bind::VertexShader*) {}
		);
		delete[] shaderPath.m_params.asShader.IALayoutInputElements;
		/*
		UINT numInputElements = 1;
		D3D11_INPUT_ELEMENT_DESC IALayoutInputElements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		s_pointEmitterShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/EmitterDebugPass/PointEmitterVS.hlsl").c_str(),
			numInputElements,
			IALayoutInputElements);
		*/
		s_staticDataInitializated = true;
	}
}