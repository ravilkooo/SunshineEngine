#include <ParticleSystem/ParticleEmitter.h>
#include <ParticleSystem/ParticleSystem.h>
#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/GraphicsResources/Texture.h>


namespace SE
{
    ParticleEmitter::ParticleEmitter(
        ParticleSystem* particleSystem,
        ParticleData::EmitterPointConstantBuffer emitterDesc,
        ParticleData::SimulateParticlesConstantBuffer simulatorDesc
    )
        : GameObject()
    {
        m_particleData = eastl::make_unique<ParticleData>(particleSystem, emitterDesc, simulatorDesc);

        m_name = "ParticleEmitter";

    }

    ParticleEmitter::~ParticleEmitter()
    {

    }

    ParticleEmitter_Info::ParticleEmitter_Info(
        ParticleSystem* particleSystem,
        ParticleData::EmitterPointConstantBuffer emitterDesc,
        ParticleData::SimulateParticlesConstantBuffer simulatorDesc
    )
        : GameObject_Info()
    {
        m_particleData = eastl::make_unique<ParticleData>(particleSystem, emitterDesc, simulatorDesc);

        m_group = GameObjectGroup::ParticleEmitter;
        m_name = "ParticleEmitter";

    }

    ParticleEmitter_Info::~ParticleEmitter_Info()
    {

    }

    ParticleData::ParticleData(ParticleSystem* particleSystem,
        EmitterPointConstantBuffer emitterDesc,
        SimulateParticlesConstantBuffer simulatorDesc)
    {
        m_particleSystem = particleSystem;
        m_emitterConstantBufferData = emitterDesc;
        m_simulateParticlesConstantBufferData = simulatorDesc;

        auto device = m_particleSystem->m_renderer->GetDevice();

        D3D11_BUFFER_DESC particleBufferDesc;
        particleBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        particleBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        particleBufferDesc.CPUAccessFlags = 0;
        particleBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        particleBufferDesc.ByteWidth = sizeof(Particle) * m_maxParticles;
        particleBufferDesc.StructureByteStride = sizeof(Particle);

        device->CreateBuffer(&particleBufferDesc, nullptr, &m_particleBuffer);

        D3D11_UNORDERED_ACCESS_VIEW_DESC particleUAVDesc;
        particleUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        particleUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        particleUAVDesc.Buffer.FirstElement = 0;
        particleUAVDesc.Buffer.NumElements = m_maxParticles;
        particleUAVDesc.Buffer.Flags = 0;

        device->CreateUnorderedAccessView(m_particleBuffer.Get(), &particleUAVDesc, &m_particleUAV);

        D3D11_SHADER_RESOURCE_VIEW_DESC particleSRVDesc;
        particleSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        particleSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        particleSRVDesc.Buffer.FirstElement = 0;
        particleSRVDesc.Buffer.NumElements = m_maxParticles;

        device->CreateShaderResourceView(m_particleBuffer.Get(), &particleSRVDesc, &m_particleSRV);

        // deadList

        D3D11_BUFFER_DESC deadListBufferDesc;
        deadListBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        deadListBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        deadListBufferDesc.CPUAccessFlags = 0;
        deadListBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        deadListBufferDesc.ByteWidth = sizeof(UINT) * m_maxParticles;
        deadListBufferDesc.StructureByteStride = sizeof(UINT);

        device->CreateBuffer(&deadListBufferDesc, nullptr, &m_deadListBuffer);

        D3D11_UNORDERED_ACCESS_VIEW_DESC deadListUAVDesc;
        deadListUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        deadListUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        deadListUAVDesc.Buffer.FirstElement = 0;
        deadListUAVDesc.Buffer.NumElements = m_maxParticles;
        deadListUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

        device->CreateUnorderedAccessView(m_deadListBuffer.Get(), &deadListUAVDesc, &m_deadListUAV);

        // aliveList (indexes)

        D3D11_BUFFER_DESC aliveIndexBufferDesc;
        aliveIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        aliveIndexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        aliveIndexBufferDesc.CPUAccessFlags = 0;
        aliveIndexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        aliveIndexBufferDesc.ByteWidth = sizeof(ParticleIndexElement) * m_maxParticles;
        aliveIndexBufferDesc.StructureByteStride = sizeof(ParticleIndexElement);

        device->CreateBuffer(&aliveIndexBufferDesc, nullptr, &m_aliveIndexBuffer[0]);

        device->CreateBuffer(&aliveIndexBufferDesc, nullptr, &m_aliveIndexBuffer[1]);

        D3D11_UNORDERED_ACCESS_VIEW_DESC aliveIndexUAVDesc;
        aliveIndexUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        aliveIndexUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        aliveIndexUAVDesc.Buffer.FirstElement = 0;
        aliveIndexUAVDesc.Buffer.NumElements = m_maxParticles;
        aliveIndexUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

        device->CreateUnorderedAccessView(m_aliveIndexBuffer[0].Get(), &aliveIndexUAVDesc, &m_aliveIndexUAV[0]);

        device->CreateUnorderedAccessView(m_aliveIndexBuffer[1].Get(), &aliveIndexUAVDesc, &m_aliveIndexUAV[1]);

        D3D11_SHADER_RESOURCE_VIEW_DESC aliveIndexSRVDesc;
        aliveIndexSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        aliveIndexSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        aliveIndexSRVDesc.Buffer.FirstElement = 0;
        aliveIndexSRVDesc.Buffer.NumElements = m_maxParticles;

        device->CreateShaderResourceView(m_aliveIndexBuffer[0].Get(), &aliveIndexSRVDesc, &m_aliveIndexSRV[0]);

        device->CreateShaderResourceView(m_aliveIndexBuffer[1].Get(), &aliveIndexSRVDesc, &m_aliveIndexSRV[1]);

        // indirectDispatchArgs

        D3D11_BUFFER_DESC indirectDispatchArgsBufferDesc;
        ZeroMemory(&indirectDispatchArgsBufferDesc, sizeof(indirectDispatchArgsBufferDesc));
        indirectDispatchArgsBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indirectDispatchArgsBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        indirectDispatchArgsBufferDesc.ByteWidth = 3 * sizeof(UINT);
        indirectDispatchArgsBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

        device->CreateBuffer(&indirectDispatchArgsBufferDesc, nullptr, &m_indirectDispatchArgsBuffer[0]);

        device->CreateBuffer(&indirectDispatchArgsBufferDesc, nullptr, &m_indirectDispatchArgsBuffer[1]);

        D3D11_UNORDERED_ACCESS_VIEW_DESC indirectDispatchArgsUAVDesc;
        indirectDispatchArgsUAVDesc.Format = DXGI_FORMAT_R32_UINT;
        indirectDispatchArgsUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        indirectDispatchArgsUAVDesc.Buffer.FirstElement = 0;
        indirectDispatchArgsUAVDesc.Buffer.NumElements = 3;
        indirectDispatchArgsUAVDesc.Buffer.Flags = 0;

        device->CreateUnorderedAccessView(m_indirectDispatchArgsBuffer[0].Get(), &indirectDispatchArgsUAVDesc, &m_indirectDispatchArgsUAV[0]);

        device->CreateUnorderedAccessView(m_indirectDispatchArgsBuffer[1].Get(), &indirectDispatchArgsUAVDesc, &m_indirectDispatchArgsUAV[1]);


        // indirectDrawArgs

        D3D11_BUFFER_DESC indirectDrawArgsBuffer;
        ZeroMemory(&indirectDrawArgsBuffer, sizeof(indirectDrawArgsBuffer));
        indirectDrawArgsBuffer.Usage = D3D11_USAGE_DEFAULT;
        indirectDrawArgsBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        indirectDrawArgsBuffer.ByteWidth = 5 * sizeof(UINT);
        indirectDrawArgsBuffer.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

        device->CreateBuffer(&indirectDrawArgsBuffer, nullptr, &m_indirectDrawArgsBuffer);

        D3D11_UNORDERED_ACCESS_VIEW_DESC indirectDrawArgsUAVDesc;
        indirectDrawArgsUAVDesc.Format = DXGI_FORMAT_R32_UINT;
        indirectDrawArgsUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        indirectDrawArgsUAVDesc.Buffer.FirstElement = 0;
        indirectDrawArgsUAVDesc.Buffer.NumElements = 5;
        indirectDrawArgsUAVDesc.Buffer.Flags = 0;

        device->CreateUnorderedAccessView(m_indirectDrawArgsBuffer.Get(), &indirectDrawArgsUAVDesc, &m_indirectDrawArgsUAV);

        //DeadList CONSTANT BUFFER
        CD3D11_BUFFER_DESC deadListCountConstantBufferDesc(sizeof(DeadListCountConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        device->CreateBuffer(&deadListCountConstantBufferDesc, nullptr, &m_deadListCountConstantBuffer);
        device->CreateBuffer(&deadListCountConstantBufferDesc, nullptr, &m_aliveListCountConstantBuffer);

        deadListCountConstantBufferDesc.Usage = D3D11_USAGE_STAGING;
        deadListCountConstantBufferDesc.BindFlags = 0;
        deadListCountConstantBufferDesc.ByteWidth = sizeof(DeadListCountConstantBuffer);
        deadListCountConstantBufferDesc.StructureByteStride = sizeof(DeadListCountConstantBuffer);
        deadListCountConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        deadListCountConstantBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        device->CreateBuffer(&deadListCountConstantBufferDesc, nullptr, &m_deadListCountConstantBuffer_2);


        // init simulate dispatch args buffer
        CD3D11_BUFFER_DESC initSimulateDispatchArgsBufferDesc(sizeof(InitIndirectComputeArgs1DConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        device->CreateBuffer(&initSimulateDispatchArgsBufferDesc, nullptr, &m_initSimulateDispatchArgsBuffer);

        // Emitter Buffer 
        CD3D11_BUFFER_DESC emitterConstantBufferDesc(sizeof(EmitterPointConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        device->CreateBuffer(&emitterConstantBufferDesc, nullptr, &m_emitterConstantBuffer);

        // Simulator Buffer 
        CD3D11_BUFFER_DESC simulateParticlesConstantBufferDesc(sizeof(SimulateParticlesConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);

        D3D11_SUBRESOURCE_DATA InitData = {};
        InitData.pSysMem = &m_simulateParticlesConstantBufferData;
        device->CreateBuffer(&simulateParticlesConstantBufferDesc, &InitData, &m_simulateParticlesConstantBuffer);

        ResetParticlesPass();
    }

    ParticleData::~ParticleData()
    {
        m_particleBuffer.Reset();
        m_particleSRV.Reset();
        m_particleUAV.Reset();
        m_deadListBuffer.Reset();
        m_deadListUAV.Reset();

        for (int i = 0; i < 2; i++)
        {
            m_aliveIndexBuffer[i].Reset();
            m_aliveIndexSRV[i].Reset();
            m_aliveIndexUAV[i].Reset();
            m_indirectDispatchArgsBuffer[i].Reset();
            m_indirectDispatchArgsUAV[i].Reset();
        }

        m_initSimulateDispatchArgsBuffer.Reset();
        m_indirectDrawArgsBuffer.Reset();
        m_indirectDrawArgsUAV.Reset();
        m_deadListCountConstantBuffer.Reset();
        m_deadListCountConstantBuffer_2.Reset();
        m_aliveListCountConstantBuffer.Reset();
        m_emitterConstantBuffer.Reset();
        m_simulateParticlesConstantBuffer.Reset();
    }

    void ParticleData::ResetParticlesPass()
    {
        auto context = m_particleSystem->m_renderer->GetDeviceContext();

        UINT initialCount[] = { 0 };
        context->CSSetUnorderedAccessViews(0, 1, m_deadListUAV.GetAddressOf(), initialCount);
        initialCount[0] = (UINT)-1;
        context->CSSetUnorderedAccessViews(1, 1, m_particleUAV.GetAddressOf(), initialCount);
        context->CSSetUnorderedAccessViews(2, 1, m_indirectDispatchArgsUAV[m_currentAliveBuffer].GetAddressOf(), initialCount);

        context->CSSetShader(m_particleSystem->m_resetCShader.Get(), nullptr, 0);

        context->Dispatch(Align(m_maxParticles, 256u) / 256, 1, 1);

        ID3D11UnorderedAccessView* uavs[] = { nullptr,nullptr,nullptr };
        context->CSSetUnorderedAccessViews(0, 3, uavs, nullptr);
    }

    void ParticleData::EmitPass()
    {
        if (m_emitterConstantBufferData.maxSpawn == 0)
        {
            return;
        }

        auto context = m_particleSystem->m_renderer->GetDeviceContext();

        UINT initialCounts[] = { (UINT)-1, (UINT)-1, (UINT)-1, (UINT)-1 };
        ID3D11UnorderedAccessView* uavs[] = {
            m_deadListUAV.Get(),
            m_particleUAV.Get(),
            m_aliveIndexUAV[m_currentAliveBuffer].Get(),
            m_indirectDispatchArgsUAV[m_currentAliveBuffer].Get()
        };

        context->CSSetUnorderedAccessViews(0, 4, uavs, initialCounts);

        {
            //copy the deadList counter to a constantBuffer
            context->CopyStructureCount(m_deadListCountConstantBuffer.Get(), 0, m_deadListUAV.Get());

            context->CSSetConstantBuffers(1, 1, m_deadListCountConstantBuffer.GetAddressOf());

            context->UpdateSubresource(m_emitterConstantBuffer.Get(), 0, nullptr, &m_emitterConstantBufferData, 0, 0);

            context->CSSetConstantBuffers(2, 1, m_emitterConstantBuffer.GetAddressOf());

            //maxSpawn / 256 as group max so and in shader it's 256 so we spawn maxspawn aligned to 256 threads
            context->Dispatch(Align(m_emitterConstantBufferData.maxSpawn, 256u) / 256, 1, 1);
        }

        ID3D11UnorderedAccessView* uavsNull[] = { nullptr, nullptr, nullptr, nullptr };
        context->CSSetUnorderedAccessViews(0, 4, uavsNull, nullptr);
    }

    void ParticleData::InitSimDispatchArgsPass()
    {
        auto context = m_particleSystem->m_renderer->GetDeviceContext();

        //init indirect dispatch args (align)
        m_initSimulateDispatchArgsData.nbThreadGroupX = 256.0f;
        context->UpdateSubresource(m_initSimulateDispatchArgsBuffer.Get(), 0, nullptr, &m_initSimulateDispatchArgsData, 0, 0);

        context->CSSetConstantBuffers(0, 1, m_initSimulateDispatchArgsBuffer.GetAddressOf());
        UINT initCount[] = { (UINT)-1 };
        context->CSSetUnorderedAccessViews(0, 1, m_indirectDispatchArgsUAV[m_currentAliveBuffer].GetAddressOf(), initCount);
        context->Dispatch(1, 1, 1);

        ID3D11UnorderedAccessView* uavs[] = { nullptr };
        context->CSSetUnorderedAccessViews(0, 1, uavs, nullptr);
    }

    void ParticleData::SimulatePass()
    {
        //simulation
        auto context = m_particleSystem->m_renderer->GetDeviceContext();

        //m_d3dContext->UpdateSubresource(m_simulateParticlesBuffer.Get(), 0, nullptr, &m_simulateParticlesBufferData, 0, 0);

        //m_d3dContext->CSSetConstantBuffers(4, 1, m_simulateParticlesBuffer.GetAddressOf());


        context->CopyStructureCount(m_aliveListCountConstantBuffer.Get(), 0, m_aliveIndexUAV[m_currentAliveBuffer].Get());
        context->CSSetConstantBuffers(1, 1, m_aliveListCountConstantBuffer.GetAddressOf());

        context->UpdateSubresource(m_simulateParticlesConstantBuffer.Get(), 0, nullptr, &m_simulateParticlesConstantBufferData, 0, 0);
        context->CSSetConstantBuffers(2, 1, m_simulateParticlesConstantBuffer.GetAddressOf());

        UINT initialCount[] = { (UINT)-1 };
        context->CSSetUnorderedAccessViews(0, 1, m_indirectDrawArgsUAV.GetAddressOf(), initialCount);
        context->CSSetUnorderedAccessViews(2, 1, m_deadListUAV.GetAddressOf(), initialCount);
        context->CSSetUnorderedAccessViews(3, 1, m_particleUAV.GetAddressOf(), initialCount);
        context->CSSetUnorderedAccessViews(4, 1, m_aliveIndexUAV[m_currentAliveBuffer].GetAddressOf(), initialCount);
        context->CSSetUnorderedAccessViews(5, 1, m_indirectDispatchArgsUAV[(m_currentAliveBuffer + 1) % 2].GetAddressOf(), initialCount);

        initialCount[0] = 0;
        context->CSSetUnorderedAccessViews(1, 1, m_aliveIndexUAV[(m_currentAliveBuffer + 1) % 2].GetAddressOf(), initialCount);

        /*
        context->CSSetShaderResources(0, 1, m_attractorsSRV.GetAddressOf());
        context->CSSetShaderResources(1, 1, m_noiseTextureSRV.GetAddressOf());
        context->CSSetShaderResources(2, 1, m_forceFieldTextureSRV.GetAddressOf());
        switch (m_forceFieldSampleMode)
        {
        case 0:
            context->CSSetSamplers(0, 1, RenderStatesHelper::LinearWrap().GetAddressOf());
            break;
        case 1:
            context->CSSetSamplers(0, 1, RenderStatesHelper::LinearBorder().GetAddressOf());
            break;
        case 2:
            context->CSSetSamplers(0, 1, RenderStatesHelper::LinearClamp().GetAddressOf());
            break;
        default:
            break;
        }
        */

        /*
        for (auto bindable : additionalBindablesForSimulationPass) {
            bindable->Bind(context.Get());
        }
        */

        context->DispatchIndirect(m_indirectDispatchArgsBuffer[m_currentAliveBuffer].Get(), 0);

        ID3D11UnorderedAccessView* uavsNull[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
        context->CSSetUnorderedAccessViews(0, 6, uavsNull, nullptr);

        /*
        context->setSRV(0, nullptr);
        context->setSRV(1, nullptr);
        context->setSRV(2, nullptr);
        */
        context->CopyStructureCount(m_aliveListCountConstantBuffer.Get(), 0, m_aliveIndexUAV[(m_currentAliveBuffer + 1) % 2].Get());

        //increment current alive
        m_currentAliveBuffer = (m_currentAliveBuffer + 1) % 2;
    }

    void ParticleData::RenderPass()
    {
        auto context = m_particleSystem->m_renderer->GetDeviceContext();

        ID3D11ShaderResourceView* vertexShaderSRVs[] = { m_particleSRV.Get(), m_aliveIndexSRV[m_currentAliveBuffer].Get() };
        context->VSSetShaderResources(0, 2, vertexShaderSRVs);
        context->VSSetConstantBuffers(3, 1, m_aliveListCountConstantBuffer.GetAddressOf());

        //const float blendFactor[4] = { 1.f, 1.f, 1.f, 1.f };

        //context->PSSetSamplers(0, 1, RenderStatesHelper::LinearClamp().GetAddressOf());
        //context-> // m_renderParticlePS->setSRV(0, m_particleTexture1SRV);
        m_texture->Bind(context);

        context->DrawInstancedIndirect(m_indirectDrawArgsBuffer.Get(), 0);


        ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr };
        ID3D11ShaderResourceView* nullSRVsPS[] = { nullptr };
        context->VSSetShaderResources(0, 2, nullSRVs);
        context->PSSetShaderResources(0, 1, nullSRVsPS);
    }

    void ParticleData::UpdateEmitter(float deltaTime)
    {
        auto context = m_particleSystem->m_renderer->GetDeviceContext();

        m_emissionRateAccumulation += m_emissionRate * deltaTime;

        if (m_emissionRateAccumulation > 1.0f)
        {
            float integerPart = 0.0f;
            float fraction = modf(m_emissionRateAccumulation, &integerPart);

            context->CopyStructureCount(m_deadListCountConstantBuffer.Get(), 0, m_deadListUAV.Get());
            context->CopyResource(m_deadListCountConstantBuffer_2.Get(), m_deadListCountConstantBuffer.Get());
            //m_deadListCountConstantBuffer.
            D3D11_MAPPED_SUBRESOURCE mappedData;
            context->Map(m_deadListCountConstantBuffer_2.Get(), 0, D3D11_MAP_READ, 0, &mappedData);
            DeadListCountConstantBuffer* dataView = reinterpret_cast<DeadListCountConstantBuffer*>(mappedData.pData);
            UINT nbDeadParticles = dataView->nbDeadParticles;
            context->Unmap(m_deadListCountConstantBuffer_2.Get(), 0);

            m_emitterConstantBufferData.maxSpawn = (UINT)eastl::min(nbDeadParticles, integerPart);
            m_emissionRateAccumulation = fraction + (integerPart - m_emitterConstantBufferData.maxSpawn);
        }
        else
        {
            m_emitterConstantBufferData.maxSpawn = 0;
        }
    }

    void ParticleData::SetEmissionRate(float emissionRate)
    {
        float particleLifeTime = m_emitterConstantBufferData.particlesLifeSpan;
        float emissionRateLimit = m_maxParticles / particleLifeTime * 0.9f;

        m_emissionRate = eastl::max(0, eastl::min(emissionRate, emissionRateLimit));
    }

    void ParticleData::IncrementEmissionRate(float deltaEmissionRate)
    {
        SetEmissionRate(m_emissionRate + deltaEmissionRate);
    }

    void ParticleData::DecrementEmissionRate(float deltaEmissionRate)
    {
        SetEmissionRate(m_emissionRate - deltaEmissionRate);
    }

    void ParticleData::SetEmitPosition(DXSM::Vector3 newPosition)
    {
        m_emitterConstantBufferData.position = newPosition;
    }

    void ParticleData::SetEmitDir(DXSM::Vector3 newEmitDir)
    {
        m_emitterConstantBufferData.rotMatrix =
            DXSM::Matrix::CreateFromQuaternion(DXSM::Quaternion::FromToRotation({ 0,1,0 }, newEmitDir));
    }

    void ParticleData::SetTexture(eastl::unique_ptr<SE_G::Bind::Texture> newTexture)
    {
        m_texture = eastl::move(newTexture);
    }

}
