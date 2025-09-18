#include "ParticleSystem.h"
#include "Utils/wcharUtils.h"

ParticleSystem::ParticleSystem(ID3D11Device* device, ID3D11DeviceContext* context,
	EmitterPointConstantBuffer emitterDesc,
	SimulateParticlesConstantBuffer simulatorDesc)
	: m_d3dDevice(device), m_d3dContext(context)
{
	// particleBuffer

	D3D11_BUFFER_DESC particleBufferDesc;
	particleBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	particleBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	particleBufferDesc.CPUAccessFlags = 0;
	particleBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	particleBufferDesc.ByteWidth = sizeof(Particle) * m_maxParticles;
	particleBufferDesc.StructureByteStride = sizeof(Particle);

	m_d3dDevice->CreateBuffer(&particleBufferDesc, nullptr, &m_particleBuffer);

	D3D11_UNORDERED_ACCESS_VIEW_DESC particleUAVDesc;
	particleUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	particleUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	particleUAVDesc.Buffer.FirstElement = 0;
	particleUAVDesc.Buffer.NumElements = m_maxParticles;
	particleUAVDesc.Buffer.Flags = 0;

	m_d3dDevice->CreateUnorderedAccessView(m_particleBuffer.Get(), &particleUAVDesc, &m_particleUAV);

	D3D11_SHADER_RESOURCE_VIEW_DESC particleSRVDesc;
	particleSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	particleSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	particleSRVDesc.Buffer.FirstElement = 0;
	particleSRVDesc.Buffer.NumElements = m_maxParticles;

	m_d3dDevice->CreateShaderResourceView(m_particleBuffer.Get(), &particleSRVDesc, &m_particleSRV);

	// deadList

	D3D11_BUFFER_DESC deadListBufferDesc;
	deadListBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	deadListBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	deadListBufferDesc.CPUAccessFlags = 0;
	deadListBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	deadListBufferDesc.ByteWidth = sizeof(UINT) * m_maxParticles;
	deadListBufferDesc.StructureByteStride = sizeof(UINT);

	m_d3dDevice->CreateBuffer(&deadListBufferDesc, nullptr, &m_deadListBuffer);

	D3D11_UNORDERED_ACCESS_VIEW_DESC deadListUAVDesc;
	deadListUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	deadListUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	deadListUAVDesc.Buffer.FirstElement = 0;
	deadListUAVDesc.Buffer.NumElements = m_maxParticles;
	deadListUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	m_d3dDevice->CreateUnorderedAccessView(m_deadListBuffer.Get(), &deadListUAVDesc, &m_deadListUAV);

	// aliveList (indexes)

	D3D11_BUFFER_DESC aliveIndexBufferDesc;
	aliveIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	aliveIndexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	aliveIndexBufferDesc.CPUAccessFlags = 0;
	aliveIndexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	aliveIndexBufferDesc.ByteWidth = sizeof(ParticleIndexElement) * m_maxParticles;
	aliveIndexBufferDesc.StructureByteStride = sizeof(ParticleIndexElement);

	m_d3dDevice->CreateBuffer(&aliveIndexBufferDesc, nullptr, &m_aliveIndexBuffer[0]);

	m_d3dDevice->CreateBuffer(&aliveIndexBufferDesc, nullptr, &m_aliveIndexBuffer[1]);

	D3D11_UNORDERED_ACCESS_VIEW_DESC aliveIndexUAVDesc;
	aliveIndexUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	aliveIndexUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	aliveIndexUAVDesc.Buffer.FirstElement = 0;
	aliveIndexUAVDesc.Buffer.NumElements = m_maxParticles;
	aliveIndexUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	m_d3dDevice->CreateUnorderedAccessView(m_aliveIndexBuffer[0].Get(), &aliveIndexUAVDesc, &m_aliveIndexUAV[0]);

	m_d3dDevice->CreateUnorderedAccessView(m_aliveIndexBuffer[1].Get(), &aliveIndexUAVDesc, &m_aliveIndexUAV[1]);

	/*
	//Same UAV as before but for sorting
	D3D11_UNORDERED_ACCESS_VIEW_DESC aliveIndexUAVSortingDesc;
	aliveIndexUAVSortingDesc.Format = DXGI_FORMAT_UNKNOWN;
	aliveIndexUAVSortingDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	aliveIndexUAVSortingDesc.Buffer.FirstElement = 0;
	aliveIndexUAVSortingDesc.Buffer.NumElements = m_maxParticles;
	aliveIndexUAVSortingDesc.Buffer.Flags = 0;

	m_d3dDevice->CreateUnorderedAccessView(m_aliveIndexBuffer[0].Get(), &aliveIndexUAVSortingDesc, &m_aliveIndexUAVSorting[0]);

	m_d3dDevice->CreateUnorderedAccessView(m_aliveIndexBuffer[1].Get(), &aliveIndexUAVSortingDesc, &m_aliveIndexUAVSorting[1]);
	*/

	D3D11_SHADER_RESOURCE_VIEW_DESC aliveIndexSRVDesc;
	aliveIndexSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	aliveIndexSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	aliveIndexSRVDesc.Buffer.FirstElement = 0;
	aliveIndexSRVDesc.Buffer.NumElements = m_maxParticles;

	m_d3dDevice->CreateShaderResourceView(m_aliveIndexBuffer[0].Get(), &aliveIndexSRVDesc, &m_aliveIndexSRV[0]);

	m_d3dDevice->CreateShaderResourceView(m_aliveIndexBuffer[1].Get(), &aliveIndexSRVDesc, &m_aliveIndexSRV[1]);

	// indirectDispatchArgs

	D3D11_BUFFER_DESC indirectDispatchArgsBufferDesc;
	ZeroMemory(&indirectDispatchArgsBufferDesc, sizeof(indirectDispatchArgsBufferDesc));
	indirectDispatchArgsBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indirectDispatchArgsBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	indirectDispatchArgsBufferDesc.ByteWidth = 3 * sizeof(UINT);
	indirectDispatchArgsBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

	m_d3dDevice->CreateBuffer(&indirectDispatchArgsBufferDesc, nullptr, &m_indirectDispatchArgsBuffer[0]);

	m_d3dDevice->CreateBuffer(&indirectDispatchArgsBufferDesc, nullptr, &m_indirectDispatchArgsBuffer[1]);

	D3D11_UNORDERED_ACCESS_VIEW_DESC indirectDispatchArgsUAVDesc;
	indirectDispatchArgsUAVDesc.Format = DXGI_FORMAT_R32_UINT;
	indirectDispatchArgsUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	indirectDispatchArgsUAVDesc.Buffer.FirstElement = 0;
	indirectDispatchArgsUAVDesc.Buffer.NumElements = 3;
	indirectDispatchArgsUAVDesc.Buffer.Flags = 0;

	m_d3dDevice->CreateUnorderedAccessView(m_indirectDispatchArgsBuffer[0].Get(), &indirectDispatchArgsUAVDesc, &m_indirectDispatchArgsUAV[0]);

	m_d3dDevice->CreateUnorderedAccessView(m_indirectDispatchArgsBuffer[1].Get(), &indirectDispatchArgsUAVDesc, &m_indirectDispatchArgsUAV[1]);


	// indirectDrawArgs

	D3D11_BUFFER_DESC indirectDrawArgsBuffer;
	ZeroMemory(&indirectDrawArgsBuffer, sizeof(indirectDrawArgsBuffer));
	indirectDrawArgsBuffer.Usage = D3D11_USAGE_DEFAULT;
	indirectDrawArgsBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	indirectDrawArgsBuffer.ByteWidth = 5 * sizeof(UINT);
	indirectDrawArgsBuffer.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

	m_d3dDevice->CreateBuffer(&indirectDrawArgsBuffer, nullptr, &m_indirectDrawArgsBuffer);

	D3D11_UNORDERED_ACCESS_VIEW_DESC indirectDrawArgsUAVDesc;
	indirectDrawArgsUAVDesc.Format = DXGI_FORMAT_R32_UINT;
	indirectDrawArgsUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	indirectDrawArgsUAVDesc.Buffer.FirstElement = 0;
	indirectDrawArgsUAVDesc.Buffer.NumElements = 5;
	indirectDrawArgsUAVDesc.Buffer.Flags = 0;

	m_d3dDevice->CreateUnorderedAccessView(m_indirectDrawArgsBuffer.Get(), &indirectDrawArgsUAVDesc, &m_indirectDrawArgsUAV);

	//DeadList CONSTANT BUFFER
	CD3D11_BUFFER_DESC deadListCountConstantBufferDesc(sizeof(DeadListCountConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	m_d3dDevice->CreateBuffer(&deadListCountConstantBufferDesc, nullptr, &m_deadListCountConstantBuffer);
	m_d3dDevice->CreateBuffer(&deadListCountConstantBufferDesc, nullptr, &m_aliveListCountConstantBuffer);

	deadListCountConstantBufferDesc.Usage = D3D11_USAGE_STAGING;
	deadListCountConstantBufferDesc.BindFlags = 0;
	deadListCountConstantBufferDesc.ByteWidth = sizeof(DeadListCountConstantBuffer);
	deadListCountConstantBufferDesc.StructureByteStride = sizeof(DeadListCountConstantBuffer);
	deadListCountConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	deadListCountConstantBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	m_d3dDevice->CreateBuffer(&deadListCountConstantBufferDesc, nullptr, &m_deadListCountConstantBuffer_2);


	// init simulate dispatch args buffer
	CD3D11_BUFFER_DESC initSimulateDispatchArgsBufferDesc(sizeof(InitIndirectComputeArgs1DConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	m_d3dDevice->CreateBuffer(&initSimulateDispatchArgsBufferDesc, nullptr, &m_initSimulateDispatchArgsBuffer);

	// Emitter Buffer 
	m_emitterConstantBufferData = emitterDesc;

	CD3D11_BUFFER_DESC emitterConstantBufferDesc(sizeof(EmitterPointConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	m_d3dDevice->CreateBuffer(&emitterConstantBufferDesc, nullptr, &m_emitterConstantBuffer);

	// Simulator Buffer 
	m_simulateParticlesConstantBufferData = simulatorDesc;

	{
		CD3D11_BUFFER_DESC simulateParticlesConstantBufferDesc(sizeof(SimulateParticlesConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = &m_simulateParticlesConstantBufferData;
		m_d3dDevice->CreateBuffer(&simulateParticlesConstantBufferDesc, &InitData, &m_simulateParticlesConstantBuffer);
	}

	// temp
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;

	device->CreateRasterizerState(&rasterDesc, &rasterState);
	device->CreateDepthStencilState(&dsDesc, &depthState);

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(TransformsParticles) + (16 - (sizeof(TransformsParticles) % 16)); // aligned size
	cbd.StructureByteStride = 0u;
	device->CreateBuffer(&cbd, nullptr, &m_viewProjBuffer);

	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(SceneConstantBuffer) + (16 - (sizeof(SceneConstantBuffer) % 16)); // aligned size
	cbd.StructureByteStride = 0u;
	device->CreateBuffer(&cbd, nullptr, &m_sceneConstantBuffer);

	// compute shaders compile
	//LoadCS(L"Shaders/Particles/ResetCShader.hlsl", m_resetCShader.Get());
	//LoadCS(L"Shaders/Particles/InitSimulateDispatchArgsCShader.hlsl", m_initSimulateDispatchArgsCShader.Get());
	//LoadCS(L"Shaders/Particles/EmitParticlesCShader.hlsl", m_emitParticlesCShader.Get());
	//LoadCS(L"Shaders/Particles/SimulateParticlesCShader.hlsl", m_simulateParticlesCShader.Get());
	Microsoft::WRL::ComPtr<ID3DBlob> cs_blob;

	wchar_t resetCsFilePath[250];
	getGraphicsAssetPath(resetCsFilePath, 250, L"Shaders/Particles/ResetCShader.hlsl");
		D3DCompileFromFile(resetCsFilePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "cs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&cs_blob, nullptr);
	m_d3dDevice->CreateComputeShader(
		cs_blob->GetBufferPointer(),
		cs_blob->GetBufferSize(),
		nullptr,
		m_resetCShader.GetAddressOf()
	);

	wchar_t initSimDispCsFilePath[250];
	getGraphicsAssetPath(initSimDispCsFilePath, 250, L"Shaders/Particles/InitSimulateDispatchArgsCShader.hlsl");
	D3DCompileFromFile(initSimDispCsFilePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "cs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&cs_blob, nullptr);
	m_d3dDevice->CreateComputeShader(
		cs_blob->GetBufferPointer(),
		cs_blob->GetBufferSize(),
		nullptr,
		m_initSimulateDispatchArgsCShader.GetAddressOf()
	);

	wchar_t emitCsFilePath[250];
	getGraphicsAssetPath(emitCsFilePath, 250, L"Shaders/Particles/EmitParticlesCShader.hlsl");
	D3DCompileFromFile(emitCsFilePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "cs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&cs_blob, nullptr);
	m_d3dDevice->CreateComputeShader(
		cs_blob->GetBufferPointer(),
		cs_blob->GetBufferSize(),
		nullptr,
		m_emitParticlesCShader.GetAddressOf()
	);

	wchar_t simCsFilePath[250];
	getGraphicsAssetPath(simCsFilePath, 250, L"Shaders/Particles/SimulateParticlesCShader.hlsl");
	D3DCompileFromFile(simCsFilePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "cs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&cs_blob, nullptr);
	m_d3dDevice->CreateComputeShader(
		cs_blob->GetBufferPointer(),
		cs_blob->GetBufferSize(),
		nullptr,
		m_simulateParticlesCShader.GetAddressOf()
	);

	{
		wchar_t vsFilePath[250];
		getGraphicsAssetPath(vsFilePath, 250, L"Shaders/Particles/RenderParticles_VS.hlsl");
		// m_renderParticle shaders
		Microsoft::WRL::ComPtr<ID3DBlob> vs_blob;
		D3DCompileFromFile(vsFilePath,
			nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", "vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
			&vs_blob, nullptr);

		m_d3dDevice->CreateVertexShader(
			vs_blob->GetBufferPointer(),
			vs_blob->GetBufferSize(),
			nullptr,
			m_renderParticleVS.GetAddressOf()
		);


		wchar_t gsFilePath[250];
		getGraphicsAssetPath(gsFilePath, 250, L"Shaders/Particles/RenderParticles_GS.hlsl");
		// m_renderParticle shaders
		Microsoft::WRL::ComPtr<ID3DBlob> gs_blob;
		ID3DBlob* errorVertexCode = nullptr;
		HRESULT hr =
			D3DCompileFromFile(gsFilePath,
				nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"main", "gs_5_0",
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
				&gs_blob, &errorVertexCode);

		if (FAILED(hr)) {
			// If the shader failed to compile it should have written something to the error message.
			if (errorVertexCode) {
				char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

				//std::cout << compileErrors << std::endl;
			}
			// If there was  nothing in the error message then it simply could not find the shader file itself.
			else
			{
				//std::cout << L"Shaders/Particles/RenderParticles_GS.hlsl" << L" - Missing Shader File\n";
			}
		}

		m_d3dDevice->CreateGeometryShader(
			gs_blob->GetBufferPointer(),
			gs_blob->GetBufferSize(),
			nullptr,
			m_renderParticleGS.GetAddressOf()
		);

		wchar_t psFilePath[250];
		getGraphicsAssetPath(psFilePath, 250, L"Shaders/Particles/RenderParticles_PS.hlsl");
		// m_renderParticle shaders
		Microsoft::WRL::ComPtr<ID3DBlob> ps_blob;
		D3DCompileFromFile(psFilePath,
			nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", "ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
			&ps_blob, nullptr);

		m_d3dDevice->CreatePixelShader(
			ps_blob->GetBufferPointer(),
			ps_blob->GetBufferSize(),
			nullptr,
			m_renderParticlePS.GetAddressOf()
		);

		D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements = nullptr;

		device->CreateInputLayout(
			IALayoutInputElements,
			0,
			vs_blob->GetBufferPointer(),
			vs_blob->GetBufferSize(),
			&pInputLayout);
	}

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

	textureSampler = new Bind::Sampler(device, samplerDesc, 0u);

	ResetParticles();
}

void ParticleSystem::LoadCS(LPCWSTR computeFilename, ID3D11ComputeShader* m_computeShader)
{

	Microsoft::WRL::ComPtr<ID3DBlob> cs_blob;
	//D3DReadFileToBlob(computeFilename, cs_blob.GetAddressOf());
	ID3DBlob* errorVertexCode = nullptr;
	HRESULT hr =
		D3DCompileFromFile(computeFilename,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", "cs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
			&cs_blob, &errorVertexCode);

	if (FAILED(hr)) {
		// If the shader failed to compile it should have written something to the error message.
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

			//std::cout << compileErrors << std::endl;
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			//std::cout << computeFilename << L" - Missing Shader File\n";
		}
	}

	m_d3dDevice->CreateComputeShader(
		cs_blob->GetBufferPointer(),
		cs_blob->GetBufferSize(),
		nullptr,
		&m_computeShader
	);
}

void ParticleSystem::ResetParticles()
{
	UINT initialCount[] = { 0 };
	m_d3dContext->CSSetUnorderedAccessViews(0, 1, m_deadListUAV.GetAddressOf(), initialCount);
	initialCount[0] = (UINT)-1;
	m_d3dContext->CSSetUnorderedAccessViews(1, 1, m_particleUAV.GetAddressOf(), initialCount);
	m_d3dContext->CSSetUnorderedAccessViews(2, 1, m_indirectDispatchArgsUAV[m_currentAliveBuffer].GetAddressOf(), initialCount);
	m_d3dContext->CSSetShader(m_resetCShader.Get(), nullptr, 0);
	m_d3dContext->Dispatch(align(m_maxParticles, 256) / 256, 1, 1);
	m_d3dContext->CSSetShader(nullptr, nullptr, 0);
	ID3D11UnorderedAccessView* uavs[] = { nullptr,nullptr,nullptr };
	m_d3dContext->CSSetUnorderedAccessViews(0, 3, uavs, nullptr);
}

void ParticleSystem::Update(float deltaTime)
{
	UpdateEmitter(deltaTime);

	m_sceneConstantBufferData.camPosition = Vector4(camera->GetPosition());
	m_sceneConstantBufferData.dt = deltaTime;

	m_sceneConstantBufferData.rngSeed = (float)std::rand();
	{

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		m_d3dContext->Map(m_sceneConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
		memcpy(mappedResource.pData, &m_sceneConstantBufferData, sizeof(m_sceneConstantBufferData)
			+ (16 - (sizeof(m_sceneConstantBufferData) % 16))); // aligned size
		m_d3dContext->Unmap(m_sceneConstantBuffer.Get(), 0);
	}

}

void ParticleSystem::UpdateEmitter(float deltaTime)
{
	m_emissionRateAccumulation += m_emissionRate * deltaTime;

	if (m_emissionRateAccumulation > 1.0f)
	{
		float integerPart = 0.0f;
		float fraction = modf(m_emissionRateAccumulation, &integerPart);

		m_d3dContext->CopyStructureCount(m_deadListCountConstantBuffer.Get(), 0, m_deadListUAV.Get());
		m_d3dContext->CopyResource(m_deadListCountConstantBuffer_2.Get(), m_deadListCountConstantBuffer.Get());
		//m_deadLi\=stCountConstantBuffer.
		D3D11_MAPPED_SUBRESOURCE mappedData;
		m_d3dContext->Map(m_deadListCountConstantBuffer_2.Get(), 0, D3D11_MAP_READ, 0, &mappedData);
		DeadListCountConstantBuffer* dataView = reinterpret_cast<DeadListCountConstantBuffer*>(mappedData.pData);
		UINT nbDeadParticles = dataView->nbDeadParticles;
		m_d3dContext->Unmap(m_deadListCountConstantBuffer_2.Get(), 0);

		m_emitterConstantBufferData.maxSpawn = (UINT) eastl::min(nbDeadParticles, integerPart);
		m_emissionRateAccumulation = fraction + (integerPart - m_emitterConstantBufferData.maxSpawn);
	}
	else
	{
		m_emitterConstantBufferData.maxSpawn = 0;
	}
}

void ParticleSystem::Render()
{
	Emit();

	Simulate();

	Draw();
}

void ParticleSystem::Emit()
{

	if (m_emitterConstantBufferData.maxSpawn == 0)
	{
		return;
	}

	UINT initialCounts[] = { (UINT)-1, (UINT)-1, (UINT)-1, (UINT)-1 };
	ID3D11UnorderedAccessView* uavs[] = {
		m_deadListUAV.Get(),
		m_particleUAV.Get(),
		m_aliveIndexUAV[m_currentAliveBuffer].Get(),
		m_indirectDispatchArgsUAV[m_currentAliveBuffer].Get()
	};
	m_d3dContext->CSSetUnorderedAccessViews(0, 4, uavs, initialCounts);

	{
		//copy the deadList counter to a constantBuffer
		m_d3dContext->CopyStructureCount(m_deadListCountConstantBuffer.Get(), 0, m_deadListUAV.Get());

		//global constant buffers

		m_d3dContext->CSSetConstantBuffers(0, 1, m_sceneConstantBuffer.GetAddressOf());
		m_d3dContext->CSSetConstantBuffers(1, 1, m_deadListCountConstantBuffer.GetAddressOf());

		m_d3dContext->UpdateSubresource(m_emitterConstantBuffer.Get(), 0, nullptr, &m_emitterConstantBufferData, 0, 0);

		m_d3dContext->CSSetConstantBuffers(2, 1, m_emitterConstantBuffer.GetAddressOf());

		m_d3dContext->CSSetShader(m_emitParticlesCShader.Get(), nullptr, 0);
		//maxSpawn / 256 as group max so and in shader it's 256 so we spawn maxspawn aligned to 256 threads
		m_d3dContext->Dispatch(align(m_emitterConstantBufferData.maxSpawn, 256) / 256, 1, 1);
		m_d3dContext->CSSetShader(nullptr, nullptr, 0);
	}

	ID3D11UnorderedAccessView* uavsNull[] = { nullptr, nullptr, nullptr, nullptr };
	m_d3dContext->CSSetUnorderedAccessViews(0, 4, uavsNull, nullptr);
}

void ParticleSystem::Simulate()
{
	//init indirect dispatch args (align)
	m_initSimulateDispatchArgsData.nbThreadGroupX = 256.0f;
	m_d3dContext->UpdateSubresource(m_initSimulateDispatchArgsBuffer.Get(), 0, nullptr, &m_initSimulateDispatchArgsData, 0, 0);

	m_d3dContext->CSSetConstantBuffers(0, 1, m_initSimulateDispatchArgsBuffer.GetAddressOf());
	UINT initCount[] = { (UINT)-1 };
	m_d3dContext->CSSetUnorderedAccessViews(0, 1, m_indirectDispatchArgsUAV[m_currentAliveBuffer].GetAddressOf(), initCount);
	m_d3dContext->CSSetShader(m_initSimulateDispatchArgsCShader.Get(), nullptr, 0);
	m_d3dContext->Dispatch(1, 1, 1);
	m_d3dContext->CSSetShader(nullptr, nullptr, 0);
	ID3D11UnorderedAccessView* uavs[] = { nullptr };
	m_d3dContext->CSSetUnorderedAccessViews(0, 1, uavs, nullptr);

	//simulation

	//m_d3dContext->UpdateSubresource(m_simulateParticlesBuffer.Get(), 0, nullptr, &m_simulateParticlesBufferData, 0, 0);

	//m_d3dContext->CSSetConstantBuffers(4, 1, m_simulateParticlesBuffer.GetAddressOf());

	m_d3dContext->CSSetConstantBuffers(0, 1, m_sceneConstantBuffer.GetAddressOf());

	m_d3dContext->CopyStructureCount(m_aliveListCountConstantBuffer.Get(), 0, m_aliveIndexUAV[m_currentAliveBuffer].Get());
	m_d3dContext->CSSetConstantBuffers(1, 1, m_aliveListCountConstantBuffer.GetAddressOf());

	m_d3dContext->CSSetConstantBuffers(2, 1, m_simulateParticlesConstantBuffer.GetAddressOf());

	UINT initialCount[] = { (UINT)-1 };
	m_d3dContext->CSSetUnorderedAccessViews(0, 1, m_indirectDrawArgsUAV.GetAddressOf(), initialCount);
	m_d3dContext->CSSetUnorderedAccessViews(2, 1, m_deadListUAV.GetAddressOf(), initialCount);
	m_d3dContext->CSSetUnorderedAccessViews(3, 1, m_particleUAV.GetAddressOf(), initialCount);
	m_d3dContext->CSSetUnorderedAccessViews(4, 1, m_aliveIndexUAV[m_currentAliveBuffer].GetAddressOf(), initialCount);
	m_d3dContext->CSSetUnorderedAccessViews(5, 1, m_indirectDispatchArgsUAV[(m_currentAliveBuffer + 1) % 2].GetAddressOf(), initialCount);

	initialCount[0] = 0;
	m_d3dContext->CSSetUnorderedAccessViews(1, 1, m_aliveIndexUAV[(m_currentAliveBuffer + 1) % 2].GetAddressOf(), initialCount);
	/*
	m_d3dContext->CSSetShaderResources(0, 1, m_attractorsSRV.GetAddressOf());
	m_d3dContext->CSSetShaderResources(1, 1, m_noiseTextureSRV.GetAddressOf());
	m_d3dContext->CSSetShaderResources(2, 1, m_forceFieldTextureSRV.GetAddressOf());
	switch (m_forceFieldSampleMode)
	{
	case 0:
		m_d3dContext->CSSetSamplers(0, 1, RenderStatesHelper::LinearWrap().GetAddressOf());
		break;
	case 1:
		m_d3dContext->CSSetSamplers(0, 1, RenderStatesHelper::LinearBorder().GetAddressOf());
		break;
	case 2:
		m_d3dContext->CSSetSamplers(0, 1, RenderStatesHelper::LinearClamp().GetAddressOf());
		break;
	default:
		break;
	}
	*/
	m_d3dContext->CSSetShader(m_simulateParticlesCShader.Get(), nullptr, 0);

	for (auto bindable : additionalBindablesForSimulationPass) {
		bindable->Bind(m_d3dContext.Get());
	}

	m_d3dContext->DispatchIndirect(m_indirectDispatchArgsBuffer[m_currentAliveBuffer].Get(), 0);
	m_d3dContext->CSSetShader(nullptr, nullptr, 0);

	ID3D11UnorderedAccessView* uavsNull[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	m_d3dContext->CSSetUnorderedAccessViews(0, 6, uavsNull, nullptr);
	/*
	m_d3dContext->setSRV(0, nullptr);
	m_d3dContext->setSRV(1, nullptr);
	m_d3dContext->setSRV(2, nullptr);
	*/
	m_d3dContext->CopyStructureCount(m_aliveListCountConstantBuffer.Get(), 0, m_aliveIndexUAV[(m_currentAliveBuffer + 1) % 2].Get());

	//increment current alive
	m_currentAliveBuffer = (m_currentAliveBuffer + 1) % 2;
}

void ParticleSystem::Draw()
{
	m_d3dContext->VSSetShader(m_renderParticleVS.Get(), nullptr, 0u);
	m_d3dContext->GSSetShader(m_renderParticleGS.Get(), nullptr, 0u);
	m_d3dContext->PSSetShader(m_renderParticlePS.Get(), nullptr, 0u);

	ID3D11Buffer* nullVertexBuffer = nullptr;
	UINT stride = 0;
	UINT offset = 0;

	m_d3dContext->IASetInputLayout(pInputLayout.Get());
	m_d3dContext->IASetVertexBuffers(0, 1, &nullVertexBuffer, &stride, &offset);
	m_d3dContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	ID3D11ShaderResourceView* vertexShaderSRVs[] = { m_particleSRV.Get(), m_aliveIndexSRV[m_currentAliveBuffer].Get() };
	m_d3dContext->VSSetShaderResources(0, 2, vertexShaderSRVs);
	m_d3dContext->VSSetConstantBuffers(3, 1, m_aliveListCountConstantBuffer.GetAddressOf());
	{
		const auto vMat = camera->GetViewMatrix();
		const auto pMat = camera->GetProjectionMatrix();
		const auto vpMat = vMat * pMat;

		const TransformsParticles tf = {
				vMat, pMat, vpMat
		};

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		m_d3dContext->Map(m_viewProjBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
		memcpy(mappedResource.pData, &tf, sizeof(tf) + (16 - (sizeof(tf) % 16))); // aligned size
		m_d3dContext->Unmap(m_viewProjBuffer.Get(), 0);

		m_d3dContext->GSSetConstantBuffers(0, 1, m_viewProjBuffer.GetAddressOf());
	}

	//const float blendFactor[4] = { 1.f, 1.f, 1.f, 1.f };


	m_d3dContext->OMSetDepthStencilState(depthState, 0);
	m_d3dContext->RSSetState(rasterState);
	m_blendState->Bind(m_d3dContext.Get());

	//m_d3dContext->PSSetSamplers(0, 1, RenderStatesHelper::LinearClamp().GetAddressOf());
	//m_d3dContext-> // m_renderParticlePS->setSRV(0, m_particleTexture1SRV);
	m_texture->Bind(m_d3dContext.Get());
	textureSampler->Bind(m_d3dContext.Get());

	m_d3dContext->DrawInstancedIndirect(m_indirectDrawArgsBuffer.Get(), 0);


	ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr };
	ID3D11ShaderResourceView* nullSRVsPS[] = { nullptr };
	m_d3dContext->VSSetShaderResources(0, 2, nullSRVs);
	m_d3dContext->PSSetShaderResources(0, 1, nullSRVsPS);
	//m_d3dContext->ClearState();
}

void ParticleSystem::SetEmissionRate(float emissionRate)
{
	m_emissionRate = eastl::max(0, eastl::min(emissionRate, m_maxParticles * 1.f / 4.0f));
}

void ParticleSystem::IncrementEmissionRate(float deltaEmissionRate)
{
	SetEmissionRate(m_emissionRate + deltaEmissionRate);
}

void ParticleSystem::DecrementEmissionRate(float deltaEmissionRate)
{
	SetEmissionRate(m_emissionRate - deltaEmissionRate);
}

void ParticleSystem::SetBlendState(Bind::BlendState* newBlendState)
{
	m_blendState = newBlendState;
}

void ParticleSystem::SetTexture(Bind::TextureB* newTexture)
{
	m_texture = newTexture;
}

void ParticleSystem::SetEmitPosition(Vector4 newPosition)
{
	m_emitterConstantBufferData.position = newPosition;
}

void ParticleSystem::SetEmitDir(Vector3 newEmitDir)
{
	m_emitterConstantBufferData.rotMatrix =
		Matrix::CreateFromQuaternion(Quaternion::FromToRotation({ 0,1,0 }, newEmitDir));
}
