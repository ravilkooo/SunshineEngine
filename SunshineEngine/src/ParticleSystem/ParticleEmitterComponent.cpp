#include <ParticleSystem/ParticleEmitterComponent.h>
#include <ParticleSystem/ParticleSystem.h>
#include <ParticleSystem/Particle.h>

#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/GraphicsResources/Texture.h>

#include <Graphics/Bindable/TransformCBuffer.h>
#include <Graphics/Bindable/ConstantBuffer.h>

#include <Component/TransformComponent.h>

#include <Utils/AssetPath.h>

#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

#include <ResourceManager/ResourceManagerFacade.h>

ParticleEmitterComponent::ParticleEmitterComponent()
{
}

ParticleEmitterComponent::ParticleEmitterComponent(
    SE::UUID objectUUID, TransformComponent* tc,
    SE::ParticleSystem* particleSystem,
    SE::ParticleData::EmitterPointConstantBuffer emitterDesc,
    SE::ParticleData::SimulateParticlesConstantBuffer simulatorDesc
)
{
    m_particleData = eastl::make_shared<SE::ParticleData>(particleSystem, emitterDesc, simulatorDesc);
    m_particleData->m_transformComp = tc;
    particleSystem->AddEmitter(objectUUID, m_particleData);

    // m_name = "ParticleEmitterComponent";
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{

}

void ParticleEmitterComponent::FromJson(const json& j,
    SE::UUID objectUUID, TransformComponent* tc,
    SE::ParticleSystem* particleSystem)
{
    // obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
    // obj->m_name = j["m_name"].get<std::string>().c_str();

    if (j.contains("emitterData"))
    {
        m_particleData = SE::ParticleData::FromJson(j["emitterData"], particleSystem);
    }
    else
    {
        json je;
        m_particleData = SE::ParticleData::FromJson(je, particleSystem);
    }

    m_particleData->EnableEmission();
    m_particleData->m_transformComp = tc;
    particleSystem->AddEmitter(objectUUID, m_particleData);
}

ParticleEmitterComponent_Info::ParticleEmitterComponent_Info()
{
}

ParticleEmitterComponent_Info::ParticleEmitterComponent_Info(
    SE::UUID objectUUID, TransformComponent* tc,
    SE::ParticleSystem* particleSystem,
    SE::ParticleData::EmitterPointConstantBuffer emitterDesc,
    SE::ParticleData::SimulateParticlesConstantBuffer simulatorDesc
)
{
    m_particleData = eastl::make_shared<SE::ParticleData>(particleSystem, emitterDesc, simulatorDesc);
    m_particleData->m_transformComp = tc;
    particleSystem->AddEmitter(objectUUID, m_particleData);
}

ParticleEmitterComponent_Info::~ParticleEmitterComponent_Info()
{

}

json ParticleEmitterComponent_Info::ToJson() const {
    json j;
    j["emitterData"] = m_particleData->ToJson();
    return j;
}

void ParticleEmitterComponent_Info::FromJson(const json& j,
    SE::UUID objectUUID, TransformComponent* tc,
    SE::ParticleSystem* particleSystem)
{
    if (j.contains("emitterData"))
    {
        m_particleData = SE::ParticleData::FromJson(j["emitterData"], particleSystem);
    }
    else
    {
        json je;
        m_particleData = SE::ParticleData::FromJson(je, particleSystem);
    }

    m_particleData->m_transformComp = tc;
    particleSystem->AddEmitter(objectUUID, m_particleData);
}

namespace SE
{
    ParticleData::ParticleData()
    { }

    ParticleData::ParticleData(ParticleSystem* particleSystem,
        EmitterPointConstantBuffer emitterDesc,
        SimulateParticlesConstantBuffer simulatorDesc)
    {
        m_particleSystem = particleSystem;
        m_emitterConstantBufferData = emitterDesc;
        m_simulateParticlesConstantBufferData = simulatorDesc;

        InitGraphicsResources();

        auto& rm = ResourceManagerFacade::Instance();
        AssetPath texPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
        ResourceHandle texHandle = rm.LoadByPath(texPath);
        SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

        auto particleTex = eastl::shared_ptr<SE_G::Bind::Texture>(
            texRes,
            [](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
        SetTexture(particleTex);

        SetEmissionRate(40);
    }

    void ParticleData::InitGraphicsResources()
    {
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

        // Reset alive list counters so subsequent appends start at 0
        UINT zeroCount[] = { 0 };
        context->CSSetUnorderedAccessViews(3, 1, m_aliveIndexUAV[0].GetAddressOf(), zeroCount);
        context->CSSetUnorderedAccessViews(4, 1, m_aliveIndexUAV[1].GetAddressOf(), zeroCount);

        context->CSSetShader(m_particleSystem->m_resetCShader.Get(), nullptr, 0);

        context->Dispatch(Align(m_maxParticles, 256u) / 256, 1, 1);

        ID3D11UnorderedAccessView* uavs[] = { nullptr,nullptr,nullptr,nullptr,nullptr };
        context->CSSetUnorderedAccessViews(0, 5, uavs, nullptr);
    }

    void ParticleData::EmitPass()
    {
        if (m_enabled)
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

            m_transformComp->transformBuffer->Update(context);
            context->CSSetConstantBuffers(0u, 1u,
                m_transformComp->transformBuffer->pVcbuf->pConstantBuffer.GetAddressOf());

            //copy the deadList counter to a constantBuffer
            context->CopyStructureCount(m_deadListCountConstantBuffer.Get(), 0, m_deadListUAV.Get());

            context->CSSetConstantBuffers(2, 1, m_deadListCountConstantBuffer.GetAddressOf());

            context->UpdateSubresource(m_emitterConstantBuffer.Get(), 0, nullptr, &m_emitterConstantBufferData, 0, 0);

            context->CSSetConstantBuffers(3, 1, m_emitterConstantBuffer.GetAddressOf());

            //maxSpawn / 256 as group max so and in shader it's 256 so we spawn maxspawn aligned to 256 threads
            context->Dispatch(Align(m_emitterConstantBufferData.maxSpawn, 256u) / 256, 1, 1);

            ID3D11UnorderedAccessView* uavsNull[] = { nullptr, nullptr, nullptr, nullptr };
            context->CSSetUnorderedAccessViews(0, 4, uavsNull, nullptr);
        }
    }

    void ParticleData::InitSimDispatchArgsPass()
    {
        if (m_enabled)
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
    }

    void ParticleData::SimulatePass()
    {
        if (m_enabled)
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
    }

    void ParticleData::RenderPass()
    {
        if (m_enabled)
        {
            auto context = m_particleSystem->m_renderer->GetDeviceContext();

            ID3D11ShaderResourceView* vertexShaderSRVs[] = { m_particleSRV.Get(), m_aliveIndexSRV[m_currentAliveBuffer].Get() };
            context->VSSetShaderResources(0, 2, vertexShaderSRVs);
            context->VSSetConstantBuffers(3, 1, m_aliveListCountConstantBuffer.GetAddressOf());

            //const float blendFactor[4] = { 1.f, 1.f, 1.f, 1.f };

            //context->PSSetSamplers(0, 1, RenderStatesHelper::LinearClamp().GetAddressOf());
            //context-> // m_renderParticlePS->setSRV(0, m_particleTexture1SRV);
            m_texture->Bind(context, 0u);

            context->DrawInstancedIndirect(m_indirectDrawArgsBuffer.Get(), 0);


            ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr };
            ID3D11ShaderResourceView* nullSRVsPS[] = { nullptr };
            context->VSSetShaderResources(0, 2, nullSRVs);
            context->PSSetShaderResources(0, 1, nullSRVsPS);
        }
    }

    void ParticleData::UpdateEmitter(float deltaTime)
    {
        if (m_enabled)
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
    }

    void ParticleData::SetEmissionRate(float emissionRate)
    {
        float particleLifeTime = m_emitterConstantBufferData.particlesLifeSpan;
        float emissionRateLimit = m_maxParticles / particleLifeTime * 0.9f;

        m_emissionRate = eastl::max(0, eastl::min(emissionRate, emissionRateLimit));
    }

    void ParticleData::EnableEmission()
    {
        m_enabled = true;
        SetEmissionRate(m_deaultEmissionRate);
        ResetParticlesPass();
        SetEmissionRate(m_deaultEmissionRate);
    }

    void ParticleData::DisableEmission()
    {
        m_enabled = false;
        SetEmissionRate(0u);
        ResetParticlesPass();
        SetEmissionRate(0u);
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

    void ParticleData::SetTexture(eastl::shared_ptr<SE_G::Bind::Texture> newTexture)
    {
        m_texture = newTexture;
    }

    eastl::shared_ptr<ParticleData> ParticleData::FromJson(const json& j, ParticleSystem* particleSystem)
    {
        auto particleData = eastl::make_shared<ParticleData>();

        particleData->m_particleSystem = particleSystem;
        // Deserialize particle system data

        // Optional: max particles and emission rate
        if (j.contains("MaxParticles") && j["MaxParticles"].is_number_unsigned()) {
            particleData->m_maxParticles = j["MaxParticles"].get<uint32_t>();
        }
        if (j.contains("EmissionRate") && j["EmissionRate"].is_number()) {
            particleData->m_emissionRate = j["EmissionRate"].get<float>();
            particleData->m_deaultEmissionRate = particleData->m_emissionRate;
        }

        // Emitter description
        if (j.contains("Emitter") && j["Emitter"].is_object()) {
            const auto& je = j["Emitter"];

            if (je.contains("position") && je["position"].is_array() && je["position"].size() >= 3) {
                particleData->m_emitterConstantBufferData.position.x = je["position"][0].get<float>();
                particleData->m_emitterConstantBufferData.position.y = je["position"][1].get<float>();
                particleData->m_emitterConstantBufferData.position.z = je["position"][2].get<float>();
            }
            if (je.contains("emitterSize") && je["emitterSize"].is_array() && je["emitterSize"].size() >= 3) {
                particleData->m_emitterConstantBufferData.emitterSize.x = je["emitterSize"][0].get<float>();
                particleData->m_emitterConstantBufferData.emitterSize.y = je["emitterSize"][1].get<float>();
                particleData->m_emitterConstantBufferData.emitterSize.z = je["emitterSize"][2].get<float>();
            }
            if (je.contains("particlesLifeSpan") && je["particlesLifeSpan"].is_number())
                particleData->m_emitterConstantBufferData.particlesLifeSpan = je["particlesLifeSpan"].get<float>();

            if (je.contains("colorStart") && je["colorStart"].is_array() && je["colorStart"].size() >= 3) {
                particleData->m_emitterConstantBufferData.colorStart.x = je["colorStart"][0].get<float>();
                particleData->m_emitterConstantBufferData.colorStart.y = je["colorStart"][1].get<float>();
                particleData->m_emitterConstantBufferData.colorStart.z = je["colorStart"][2].get<float>();
            }
            if (je.contains("particlesBaseSpeed") && je["particlesBaseSpeed"].is_number())
                particleData->m_emitterConstantBufferData.particlesBaseSpeed = je["particlesBaseSpeed"].get<float>();

            if (je.contains("colorEnd") && je["colorEnd"].is_array() && je["colorEnd"].size() >= 3) {
                particleData->m_emitterConstantBufferData.colorEnd.x = je["colorEnd"][0].get<float>();
                particleData->m_emitterConstantBufferData.colorEnd.y = je["colorEnd"][1].get<float>();
                particleData->m_emitterConstantBufferData.colorEnd.z = je["colorEnd"][2].get<float>();
            }
            if (je.contains("particlesMass") && je["particlesMass"].is_number())
                particleData->m_emitterConstantBufferData.particlesMass = je["particlesMass"].get<float>();

            if (je.contains("particleSizeStart") && je["particleSizeStart"].is_number())
                particleData->m_emitterConstantBufferData.particleSizeStart = je["particleSizeStart"].get<float>();
            if (je.contains("particleSizeEnd") && je["particleSizeEnd"].is_number())
                particleData->m_emitterConstantBufferData.particleSizeEnd = je["particleSizeEnd"].get<float>();

            if (je.contains("alphaStart") && je["alphaStart"].is_number())
                particleData->m_emitterConstantBufferData.alphaStart = je["alphaStart"].get<float>();
            if (je.contains("alphaEnd") && je["alphaEnd"].is_number())
                particleData->m_emitterConstantBufferData.alphaEnd = je["alphaEnd"].get<float>();

            if (je.contains("longitudeMin") && je["longitudeMin"].is_number())
                particleData->m_emitterConstantBufferData.longitudeMin = je["longitudeMin"].get<float>();
            if (je.contains("longitudeMax") && je["longitudeMax"].is_number())
                particleData->m_emitterConstantBufferData.longitudeMax = je["longitudeMax"].get<float>();

            if (je.contains("latitudeMin") && je["latitudeMin"].is_number())
                particleData->m_emitterConstantBufferData.latitudeMin = je["latitudeMin"].get<float>();
            if (je.contains("latitudeMax") && je["latitudeMax"].is_number())
                particleData->m_emitterConstantBufferData.latitudeMax = je["latitudeMax"].get<float>();

            if (je.contains("maxSpawn") && je["maxSpawn"].is_number_unsigned())
                particleData->m_emitterConstantBufferData.maxSpawn = je["maxSpawn"].get<uint32_t>();

            // Rotation: accept either a 16-element matrix or an emitDir vector
            if (je.contains("rotMatrix") && je["rotMatrix"].is_array() && je["rotMatrix"].size() == 16) {
                // Fill row-major
                auto& M = particleData->m_emitterConstantBufferData.rotMatrix;
                int idx = 0;
                for (int r = 0; r < 4; ++r) {
                    for (int c = 0; c < 4; ++c) {
                        M(r, c) = je["rotMatrix"][idx++].get<float>();
                    }
                }
            }
            else if (je.contains("emitDir") && je["emitDir"].is_array() && je["emitDir"].size() >= 3) {
                DXSM::Vector3 dir;
                dir.x = je["emitDir"][0].get<float>();
                dir.y = je["emitDir"][1].get<float>();
                dir.z = je["emitDir"][2].get<float>();
                particleData->m_emitterConstantBufferData.rotMatrix = DXSM::Matrix::CreateFromQuaternion(
                    DXSM::Quaternion::FromToRotation({ 0, 1, 0 }, dir));
            }
        }

        // Simulation description
        if (j.contains("Simulate") && j["Simulate"].is_object()) {
            const auto& js = j["Simulate"];
            if (js.contains("force") && js["force"].is_array() && js["force"].size() >= 3) {
                particleData->m_simulateParticlesConstantBufferData.force.x = js["force"][0].get<float>();
                particleData->m_simulateParticlesConstantBufferData.force.y = js["force"][1].get<float>();
                particleData->m_simulateParticlesConstantBufferData.force.z = js["force"][2].get<float>();
            }
        }

        // Texture
        auto device = particleData->m_particleSystem->m_renderer->GetDevice();
        AssetPath texPath;
        if (j.contains("Texture")) {
            texPath.FromJson(j["Texture"]);
        }
        else {
            texPath = AssetPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
        }
        auto& rm = ResourceManagerFacade::Instance();
        ResourceHandle texHandle = rm.LoadByPath(texPath);
        if (texHandle.guid == 0) {
            // Error
            auto ap = AssetPath(
                SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
                AssetPath::AssetSource::Engine);
            texHandle = ResourceManagerFacade::Instance().LoadByPath(ap);
        }
        SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

        auto particleTex = eastl::shared_ptr<SE_G::Bind::Texture>(
            texRes,
            [](SE_G::Bind::Texture*) {});
        particleData->SetTexture(particleTex);
        /*
        auto device = particleData->m_particleSystem->m_renderer->GetDevice();
        auto tex = eastl::make_shared<SE_G::Bind::Texture>(device, texPath, 0u, SE_G::Bind::PipelineStage::PIXEL_SHADER);
        particleData->SetTexture(tex);
        */

        particleData->InitGraphicsResources();

        return particleData;
    }

    json ParticleData::ToJson() const
    {
        json j;

        j["MaxParticles"] = m_maxParticles;
        j["EmissionRate"] = m_deaultEmissionRate;

        // Texture path if available
        if (m_texture) {
            j["Texture"] = m_texture->GetCurrentTexturePath().ToJson();
        }

        // Emitter
        const auto& E = m_emitterConstantBufferData;
        json je;
        je["position"] = { E.position.x, E.position.y, E.position.z };
        je["emitterSize"] = { E.emitterSize.x, E.emitterSize.y, E.emitterSize.z };
        je["colorStart"] = { E.colorStart.x, E.colorStart.y, E.colorStart.z };
        je["colorEnd"] = { E.colorEnd.x, E.colorEnd.y, E.colorEnd.z };

        je["alphaStart"] = E.alphaStart;
        je["alphaEnd"] = E.alphaEnd;

        je["particlesLifeSpan"] = E.particlesLifeSpan;
        je["particlesBaseSpeed"] = E.particlesBaseSpeed;
        je["particlesMass"] = E.particlesMass;
        je["particleSizeStart"] = E.particleSizeStart;
        je["particleSizeEnd"] = E.particleSizeEnd;
        je["longitudeMin"] = E.longitudeMin;
        je["longitudeMax"] = E.longitudeMax;
        je["latitudeMin"] = E.latitudeMin;
        je["latitudeMax"] = E.latitudeMax;
        je["maxSpawn"] = E.maxSpawn;

        // rotMatrix 4x4 row-major
        json mat = json::array();
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                mat.push_back(E.rotMatrix(r, c));
            }
        }
        je["rotMatrix"] = mat;

        j["Emitter"] = je;

        // Simulation
        const auto& S = m_simulateParticlesConstantBufferData;
        json js;
        js["force"] = { S.force.x, S.force.y, S.force.z };
        j["Simulate"] = js;

        return j;
    }
}

#define PEC_ADD_METHOD(k, fn) k, fn

LUA_REGISTER_COMPONENT(
    ParticleEmitterComponent,
    "ParticleEmitterComponent",
    /* no fields */,
    /* no properties */,
    PARTICLE_EMITTER_COMPONENT_LUA_METHODS_APPLY(PEC_ADD_METHOD),
    "getParticleEmitter")
#undef PEC_ADD_METHOD
