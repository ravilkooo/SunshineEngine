#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <SimpleMath.h>
#include <EASTL/shared_ptr.h>

#include <Utils/UUID.h>
#include <Component/Component.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace DX = DirectX;
namespace DXSM = DirectX::SimpleMath;

class TransformComponent;

namespace SE_G
{
    class DeferredRenderer;
    namespace Bind
    {
        class Texture;
    }
}

namespace SE
{
    class ParticleSystem;

    class ParticleData
    {
    public:
        static constexpr uint32_t Align(uint32_t value, uint32_t alignment) { return (value + (alignment - 1)) & ~(alignment - 1); };

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_particleBuffer;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_particleSRV;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_particleUAV;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_deadListBuffer;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_deadListUAV;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_aliveIndexBuffer[2];
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_aliveIndexSRV[2];
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_aliveIndexUAV[2];
        
        int                                                 m_currentAliveBuffer = 0;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indirectDispatchArgsBuffer[2];
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_indirectDispatchArgsUAV[2];

        struct InitIndirectComputeArgs1DConstantBuffer
        {
            float nbThreadGroupX;

            uint32_t padding[3];
        };
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_initSimulateDispatchArgsBuffer;
        InitIndirectComputeArgs1DConstantBuffer             m_initSimulateDispatchArgsData;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indirectDrawArgsBuffer;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_indirectDrawArgsUAV;


        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_deadListCountConstantBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_deadListCountConstantBuffer_2;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_aliveListCountConstantBuffer;


        struct DeadListCountConstantBuffer
        {
            uint32_t nbDeadParticles;

            uint32_t padding[3];
        };

        uint32_t m_maxParticles = 256u;

        bool m_enabled = false;
        float m_deaultEmissionRate = 0.0f;
        float m_emissionRate = 0.0f;
        float m_emissionRateAccumulation = 0.0f;

        struct EmitterPointConstantBuffer
        {
            DXSM::Matrix rotMatrix;

            DXSM::Vector3 position;
            float particlesLifeSpan;
            
            DXSM::Vector3 colorStart;
            float alphaStart;

            DXSM::Vector3 colorEnd;
            float alphaEnd;

            float particlesBaseSpeed;
            float particlesMass;
            float particleSizeStart;
            float particleSizeEnd;

            float longitudeMin;
            float longitudeMax;
            float latitudeMin;
            float latitudeMax;

            uint32_t maxSpawn;
            DXSM::Vector3 emitterPadding;

            /*
            float particleScreenSpinSpeed;
            float particleWorldSpinSpeed;
            uint32_t particleOrientation;
            */

        };

        EmitterPointConstantBuffer  m_emitterConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>    m_emitterConstantBuffer;

        struct SimulateParticlesConstantBuffer
        {
            DXSM::Vector3 force;
            float pad;
        };

        SimulateParticlesConstantBuffer  m_simulateParticlesConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>    m_simulateParticlesConstantBuffer;

        eastl::shared_ptr<SE_G::Bind::Texture> m_texture;

        TransformComponent* m_transformComp = nullptr;

        ParticleSystem* m_particleSystem;

        ParticleData();
        ParticleData(ParticleSystem* particleSystem,
            EmitterPointConstantBuffer emitterDesc,
            SimulateParticlesConstantBuffer simulatorDesc);
        ~ParticleData();

        void InitGraphicsResources();

        void UpdateEmitter(float deltaTime);

        void ResetParticlesPass();

        void EmitPass();
        void InitSimDispatchArgsPass();
        void SimulatePass();

        void RenderPass();

        void SetEmissionRate(float emissionRate);

        void EnableEmission();
        void DisableEmission();

        void IncrementEmissionRate(float deltaEmissionRate);
        void DecrementEmissionRate(float deltaEmissionRate);

        void SetEmitPosition(DXSM::Vector3 newPosition);
        void SetEmitDir(DXSM::Vector3 newEmitDir);

        void SetTexture(eastl::shared_ptr<SE_G::Bind::Texture> newTexture);

        static eastl::shared_ptr<ParticleData> FromJson(const json& j, ParticleSystem* particleSystem);
        json ToJson() const;
    };
}

class ParticleEmitterComponent :
    public Component
{
public:
    eastl::shared_ptr<SE::ParticleData> m_particleData;

    ParticleEmitterComponent();

    ParticleEmitterComponent(
        SE::UUID objectUUID, TransformComponent* tc,
        SE::ParticleSystem* particleSystem,
        SE::ParticleData::EmitterPointConstantBuffer emitterDesc,
        SE::ParticleData::SimulateParticlesConstantBuffer simulatorDesc);

    ~ParticleEmitterComponent();

    void FromJson(
        const json& j,
        SE::UUID objectUUID, TransformComponent* tc,
        SE::ParticleSystem* particleSystem);

    const std::type_info& getType() const override {
        return typeid(ParticleEmitterComponent);
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::PARTICLE_EMITTER;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }
};

class ParticleEmitterComponent_Info :
    public Component_Info
    //public GameObject
{
public:
    eastl::shared_ptr<SE::ParticleData> m_particleData;

    ParticleEmitterComponent_Info();

    ParticleEmitterComponent_Info(
        SE::UUID objectUUID, TransformComponent* tc,
        SE::ParticleSystem* particleSystem,
        SE::ParticleData::EmitterPointConstantBuffer emitterDesc,
        SE::ParticleData::SimulateParticlesConstantBuffer simulatorDesc);

    ~ParticleEmitterComponent_Info();

    json ToJson() const override;

    void FromJson(
        const json& j,
        SE::UUID objectUUID, TransformComponent* tc,
        SE::ParticleSystem* particleSystem);

    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }
    static const SE::ComponentType s_componentType = SE::ComponentType::PARTICLE_EMITTER;
    const std::type_info& getType() const override {
        return typeid(ParticleEmitterComponent_Info);
    }

    bool IsAssigned() override { return false; }
};
