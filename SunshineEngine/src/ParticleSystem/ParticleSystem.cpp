#include <ParticleSystem/ParticleSystem.h>

#include <d3dcompiler.h>

#include <ParticleSystem/Particle.h>
#include <ParticleSystem/ParticleEmitterComponent.h>

#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Bindable/Sampler.h>
#include <Graphics/Bindable/BlendState.h>

#include <Utils/AssetPath.h>

#include <Component/TransformComponent.h>
#include <Graphics/Utils/Camera.h>

#include <cstdlib>
#include <ctime>

namespace SE
{
	void ParticleSystem::LoadCS(AssetPath shaderPath, Microsoft::WRL::ComPtr<ID3D11ComputeShader>& m_computeShader)
	{

		Microsoft::WRL::ComPtr<ID3DBlob> cs_blob;
		//D3DReadFileToBlob(computeFilename, cs_blob.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3DBlob> error_blob;

		HRESULT hr =
			D3DCompileFromFile(shaderPath.GetFullPath().c_str(),
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"main", "cs_5_0",
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
				cs_blob.GetAddressOf(), error_blob.GetAddressOf());

		if (FAILED(hr)) {
			wprintf(L"%ls", shaderPath.GetFullPath().c_str());
			if (error_blob) {
				char* compileErrors = (char*)(error_blob->GetBufferPointer());
				printf(" : Shader compile error: %s\n", compileErrors);
			}
			// If there was  nothing in the error message then it simply could not find the shader file itself.
			else
			{
				printf(" : Missing Shader File\n");
			}
			cs_blob.Reset();
			error_blob.Reset();
			return;
		}

		m_renderer->GetDevice()->CreateComputeShader(
			cs_blob->GetBufferPointer(),
			cs_blob->GetBufferSize(),
			nullptr,
			m_computeShader.GetAddressOf()
		);

		cs_blob.Reset();
		if (error_blob)
			error_blob.Reset();
	}

	ParticleSystem::ParticleSystem(SE_G::DeferredRenderer* renderer,
		eastl::shared_ptr<SE_G::Camera> camera)
		: m_renderer(renderer), m_camera(camera)
	{
		// Seed random number generator for particle emission
		std::srand(static_cast<unsigned int>(std::time(nullptr)));
		
		auto device = m_renderer->GetDevice();

		AssetPath shaderPath(L"Shaders/Particles/ResetCShader.hlsl");
		LoadCS(shaderPath, m_resetCShader);

		shaderPath.m_assetRelativePath =
			L"Shaders/Particles/InitSimulateDispatchArgsCShader.hlsl";
		LoadCS(shaderPath, m_initSimulateDispatchArgsCShader);

		shaderPath.m_assetRelativePath =
			L"Shaders/Particles/EmitParticlesCShader.hlsl";
		LoadCS(shaderPath, m_emitParticlesCShader);

		shaderPath.m_assetRelativePath =
			L"Shaders/Particles/SimulateParticlesCShader.hlsl";
		LoadCS(shaderPath, m_simulateParticlesCShader);

		HRESULT hr;
		Microsoft::WRL::ComPtr<ID3DBlob> error_blob;
		Microsoft::WRL::ComPtr<ID3DBlob> shaderBytecodeBlob;

		{
			shaderPath.m_assetRelativePath =
				L"Shaders/Particles/RenderParticles_VS.hlsl";

			hr = D3DCompileFromFile(
				shaderPath.GetFullPath().c_str(),
				nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"main", "vs_5_0",
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
				m_vertexShaderBytecodeBlob.GetAddressOf(), error_blob.GetAddressOf());

			if (FAILED(hr)) {
				wprintf(L"%ls", shaderPath.GetFullPath().c_str());
				if (error_blob) {
					char* compileErrors = (char*)(error_blob->GetBufferPointer());
					printf(" : Shader compile error: %s\n", compileErrors);
				}
				// If there was  nothing in the error message then it simply could not find the shader file itself.
				else
				{
					printf(" : Missing Shader File\n");
				}
				m_vertexShaderBytecodeBlob.Reset();
			}
			else
			{
				device->CreateVertexShader(
					m_vertexShaderBytecodeBlob->GetBufferPointer(),
					m_vertexShaderBytecodeBlob->GetBufferSize(),
					nullptr,
					m_renderParticleVS.GetAddressOf()
				);

				D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements = nullptr;

				device->CreateInputLayout(
					IALayoutInputElements,
					0,
					m_vertexShaderBytecodeBlob->GetBufferPointer(),
					m_vertexShaderBytecodeBlob->GetBufferSize(),
					&m_inputLayout);
			}
		}

		{
			shaderPath.m_assetRelativePath =
				L"Shaders/Particles/RenderParticles_GS.hlsl";

			hr =
				D3DCompileFromFile(
					shaderPath.GetFullPath().c_str(),
					nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
					"main", "gs_5_0",
					D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
					shaderBytecodeBlob.GetAddressOf(), error_blob.GetAddressOf());

			if (FAILED(hr)) {
				wprintf(L"%ls", shaderPath.GetFullPath().c_str());
				if (error_blob) {
					char* compileErrors = (char*)(error_blob->GetBufferPointer());
					printf(" : Shader compile error: %s\n", compileErrors);
				}
				// If there was  nothing in the error message then it simply could not find the shader file itself.
				else
				{
					printf(" : Missing Shader File\n");
				}
				return;
			}
			else
			{
				device->CreateGeometryShader(
					shaderBytecodeBlob->GetBufferPointer(),
					shaderBytecodeBlob->GetBufferSize(),
					nullptr,
					m_renderParticleGS.GetAddressOf()
				);
			}
		}

		{
			shaderPath.m_assetRelativePath =
				L"Shaders/Particles/RenderParticles_PS.hlsl";

			hr =
				D3DCompileFromFile(
					shaderPath.GetFullPath().c_str(),
					nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
					"main", "ps_5_0",
					D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
					shaderBytecodeBlob.GetAddressOf(), error_blob.GetAddressOf());

			if (FAILED(hr)) {
				wprintf(L"%ls", shaderPath.GetFullPath().c_str());
				if (error_blob) {
					char* compileErrors = (char*)(error_blob->GetBufferPointer());
					printf(" : Shader compile error: %s\n", compileErrors);
				}
				// If there was  nothing in the error message then it simply could not find the shader file itself.
				else
				{
					printf(" : Missing Shader File\n");
				}
				return;
			}
			else
			{
				device->CreatePixelShader(
					shaderBytecodeBlob->GetBufferPointer(),
					shaderBytecodeBlob->GetBufferSize(),
					nullptr,
					m_renderParticlePS.GetAddressOf()
				);
			}
		}

		shaderBytecodeBlob.Reset();
		if (error_blob)
			error_blob.Reset();

		D3D11_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.CullMode = D3D11_CULL_NONE;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		device->CreateRasterizerState(&rasterDesc, &m_rasterState);

		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		device->CreateDepthStencilState(&dsDesc, &m_depthState);

		D3D11_BLEND_DESC particleBlendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
		particleBlendDesc.RenderTarget[0].BlendEnable = TRUE;
		particleBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		particleBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		particleBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		particleBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		particleBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		particleBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		particleBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		eastl::array<FLOAT, 4> particleBlendFactor = { 1.0f, 1.0f, 1.0f, 1.0f };
		UINT sampleMask = 0xffffffff;
		m_blendState =
			eastl::make_unique<SE_G::Bind::BlendState>(renderer->GetDevice(), particleBlendDesc, particleBlendFactor, sampleMask);

		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		m_textureSampler =
			eastl::make_unique<SE_G::Bind::Sampler>(device, samplerDesc, 0u);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = ParticleData::Align(sizeof(TransformsParticles), 16u);
		cbd.StructureByteStride = 0u;
		device->CreateBuffer(&cbd, nullptr, &m_viewProjBuffer);

		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = ParticleData::Align(sizeof(SceneConstantBuffer), 16u);
		cbd.StructureByteStride = 0u;
		device->CreateBuffer(&cbd, nullptr, &m_sceneConstantBuffer);

	}

	ParticleSystem::~ParticleSystem()
	{
		m_inputLayout.Reset();
		m_vertexShaderBytecodeBlob.Reset();
		m_renderParticleVS.Reset();
		m_renderParticleGS.Reset();
		m_renderParticlePS.Reset();
		m_resetCShader.Reset();
		m_initSimulateDispatchArgsCShader.Reset();
		m_emitParticlesCShader.Reset();
		m_simulateParticlesCShader.Reset();
		m_rasterState.Reset();
		m_depthState.Reset();

		m_sceneConstantBuffer.Reset();
		m_viewProjBuffer.Reset();

		m_emitters.clear();
	}

	void ParticleSystem::AddEmitter(SE::UUID uuid, eastl::shared_ptr<ParticleData> particleData)
	{
		const SE::UUID id = uuid;
		auto [it, inserted] = m_emitters.emplace(id, nullptr);
		if (!inserted)
		{
			printf("Duplicate UUID in ParticleSystem::AddEmitter");
			return;
		}
		it->second = particleData;
	}

	void ParticleSystem::ComputePassForAllEmitters()
	{
		if (!m_enabled)
			return;

		auto context = m_renderer->GetDeviceContext();
		context->ClearState();

		// Emitter Pass
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"Emitter Compute Pass");

		context->CSSetShader(m_emitParticlesCShader.Get(), nullptr, 0);
		context->CSSetConstantBuffers(1, 1, m_sceneConstantBuffer.GetAddressOf());

		for (auto emitter : m_emitters)
		{
			emitter.second->EmitPass();
		}

		context->CSSetShader(nullptr, nullptr, 0);
		context->ClearState();
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();

		// Init simulate dispatch args
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"Init simulate dispatch args Pass");

		context->CSSetShader(m_initSimulateDispatchArgsCShader.Get(), nullptr, 0);

		for (auto emitter : m_emitters)
		{
			emitter.second->InitSimDispatchArgsPass();
		}

		context->CSSetShader(nullptr, nullptr, 0);
		context->ClearState();
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();

		// Simulate Pass
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"Simulate Pass");

		context->CSSetShader(m_simulateParticlesCShader.Get(), nullptr, 0);
		context->CSSetConstantBuffers(0, 1, m_sceneConstantBuffer.GetAddressOf());

		for (auto emitter : m_emitters)
		{
			emitter.second->SimulatePass();
		}

		context->CSSetShader(nullptr, nullptr, 0);
		context->ClearState();
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();
	}

	void ParticleSystem::Update(float deltaTime)
	{
		if (!m_enabled)
			return;

		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"ParticleSystem Update");

		UpdateAllEmitters(deltaTime);

		m_sceneConstantBufferData.camPosition = DXSM::Vector4(m_camera->GetPosition());
		m_sceneConstantBufferData.dt = deltaTime;

		m_sceneConstantBufferData.rngSeed = (float)std::rand();

		auto context = m_renderer->GetDeviceContext();

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		context->Map(m_sceneConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
		memcpy(mappedResource.pData, &m_sceneConstantBufferData, ParticleData::Align(sizeof(m_sceneConstantBufferData), 16u)); // aligned size
		context->Unmap(m_sceneConstantBuffer.Get(), 0);

		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();

		context->ClearState();
		ComputePassForAllEmitters();
		context->ClearState();
	}

	void ParticleSystem::UpdateAllEmitters(float deltaTime)
	{
		if (!m_enabled)
			return;

		for (auto emitter : m_emitters) {

			emitter.second->UpdateEmitter(deltaTime);
		}
	}

	void ParticleSystem::DisableAllEmitters()
	{
		for (auto emitter : m_emitters) {

			emitter.second->DisableEmission();
		}
	}

	void ParticleSystem::EnableAllEmitters()
	{
		for (auto emitter : m_emitters) {

			emitter.second->EnableEmission();
		}
	}

	void ParticleSystem::RenderAllEmitters()
	{
		if (!m_enabled)
			return;

		auto context = m_renderer->GetDeviceContext();

		context->VSSetShader(m_renderParticleVS.Get(), nullptr, 0u);
		context->GSSetShader(m_renderParticleGS.Get(), nullptr, 0u);
		context->PSSetShader(m_renderParticlePS.Get(), nullptr, 0u);

		ID3D11Buffer* nullVertexBuffer = nullptr;
		UINT stride = 0;
		UINT offset = 0;

		context->IASetInputLayout(m_inputLayout.Get());
		context->IASetVertexBuffers(0, 1, &nullVertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		const auto vMat = m_camera->GetViewMatrix();
		const auto pMat = m_camera->GetProjectionMatrix();
		const auto vpMat = vMat * pMat;

		const TransformsParticles tf = {
				vMat, pMat, vpMat
		};

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		context->Map(m_viewProjBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
		memcpy(mappedResource.pData, &tf, ParticleData::Align(sizeof(tf), 16)); // aligned size
		context->Unmap(m_viewProjBuffer.Get(), 0);

		context->GSSetConstantBuffers(0, 1, m_viewProjBuffer.GetAddressOf());

		context->OMSetDepthStencilState(m_depthState.Get(), 0);
		context->RSSetState(m_rasterState.Get());
		m_blendState->Bind(context);
        m_textureSampler->Bind(context);

		for (auto emitter : m_emitters) {

			emitter.second->RenderPass();
		}
	}

	void ParticleSystem::SetBlendState(eastl::unique_ptr<SE_G::Bind::BlendState> newBlendState)
	{
		m_blendState = eastl::move(newBlendState);
	}
	
	/*
	void ParticleSystem::Update(float deltaTime)
	{
		for (auto emitter : m_emitters) {

			emitter.second->Draw();
		}
	}
	*/

	void ParticleSystem::SetRenderer(SE_G::DeferredRenderer* renderer)
	{
		m_renderer = renderer;

		if (renderer)
			m_camera = renderer->GetMainCamera();
	}

	void ParticleSystem::SetCamera(eastl::shared_ptr<SE_G::Camera> camera)
	{
		m_camera = camera;
	}

	void ParticleSystem::Enable()
	{
		m_enabled = true;
	}

	void ParticleSystem::Disable()
	{
		m_enabled = false;
	}
}
