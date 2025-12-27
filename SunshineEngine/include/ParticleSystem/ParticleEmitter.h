#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <SimpleMath.h>

#include <GameObject/GameObject.h>


namespace DX = DirectX;
namespace DXSM = DirectX::SimpleMath;

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

    class ParticleEmitter :
        public GameObject
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

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indirectDispatchArgsBuffer[2];    // для комп шейдера
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_indirectDispatchArgsUAV[2];       // для рендеринга

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

        uint32_t m_maxParticles = 4 * 1024;

        float m_emissionRate = 100.0f;  // tool
        float m_emissionRateAccumulation = 0.0f;

        struct EmitterPointConstantBuffer
        {
            DXSM::Matrix rotMatrix;

            DXSM::Vector4 position;
            DXSM::Vector4 colorStart;
            DXSM::Vector4 colorEnd;

            uint32_t maxSpawn;
            float particlesLifeSpan;
            float particlesBaseSpeed;
            float particlesMass;

            float particleSizeStart;
            float particleSizeEnd;
            float longitudeMin;
            float longitudeMax;

            float latitudeMax;
            float particleScreenSpinSpeed;
            float particleWorldSpinSpeed;
            uint32_t particleOrientation;

        };

        EmitterPointConstantBuffer  m_emitterConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>    m_emitterConstantBuffer;

        struct SimulateParticlesConstantBuffer
        {
            DXSM::Vector4 force;
        };

        SimulateParticlesConstantBuffer  m_simulateParticlesConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>    m_simulateParticlesConstantBuffer;

        eastl::unique_ptr<SE_G::Bind::Texture> m_texture;

        ParticleSystem* m_particleSystem;

        ParticleEmitter(ParticleSystem* particleSystem,
            EmitterPointConstantBuffer emitterDesc,
            SimulateParticlesConstantBuffer simulatorDesc);
        ~ParticleEmitter();

        void UpdateEmitter(float deltaTime);

        void ResetParticlesPass();

        void EmitPass();
        void InitSimDispatchArgsPass();
        void SimulatePass();

        void RenderPass();

        void SetEmissionRate(float emissionRate);
        void IncrementEmissionRate(float deltaEmissionRate);
        void DecrementEmissionRate(float deltaEmissionRate);

        void SetEmitPosition(DXSM::Vector4 newPosition);
        void SetEmitDir(DXSM::Vector3 newEmitDir);

        void SetTexture(eastl::unique_ptr<SE_G::Bind::Texture> newTexture);
    };
}
