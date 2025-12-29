#include "ParticlesGlobals.h"

uint wang_hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

uint rand_xorshift(uint rng_state)
{
    // Xorshift algorithm from George Marsaglia's paper
    rng_state ^= (rng_state << 13);
    rng_state ^= (rng_state >> 17);
    rng_state ^= (rng_state << 5);
    return rng_state;
}

//random float number between 0-1
float rand_xorshift_normalized(uint rng_state)
{
    return float(rand_xorshift(rng_state)) * (1.0 / 4294967296.0);
}

cbuffer TransformCBuf : register(b0)
{
    row_major float4x4 wMat;
    row_major float4x4 wInvTransposeMat;
}

cbuffer sceneConstantBuffer : register(b1)
{
    float4 camPosition;
    float dt;
    float rngSeed;
    
    float2 padding;
};

cbuffer deadListCountConstantBuffer : register(b2)
{
    uint nbDeadParticles;

    uint3 deadListPadding;
};

cbuffer emitterPointConstantBuffer : register(b3)
{
    row_major float4x4 rotMatrix;
    
    float3 emitterPositionOffset;
    float particlesLifeSpan;
    
    float3 colorStart;
    float alphaStart;
    
    float3 colorEnd;
    float alphaEnd;
    
    float particlesBaseSpeed;
    float particlesMass;
    float particleSizeStart;
    float particleSizeEnd;
    
    float longitudeMin;
    float longitudeMax;
    float latitudeMin;
    float latitudeMax;
    
    uint emitterMaxSpawn;
    float3 emitterPadding;
};

ConsumeStructuredBuffer<uint> deadListBuffer : register(u0);
RWStructuredBuffer<Particle> particleList : register(u1);
AppendStructuredBuffer<ParticleIndexElement> aliveParticleIndex : register(u2);
RWBuffer<uint> indirectDispatchArgs : register(u3);

//spawn per batch of 256
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x < nbDeadParticles && id.x < emitterMaxSpawn)
    {
        uint rng_state = wang_hash(id.x + rngSeed);
        uint rng_state_2 = wang_hash(id.x + 3 * rngSeed);
        
        Particle p = (Particle) 0;
        
        float4 emitterPos = float4(0.0, 0.0, 0.0, 1.0);
        emitterPos = mul(emitterPos, wMat);
        emitterPos = emitterPos / emitterPos.w;
        
        p.position = emitterPos + float4(emitterPositionOffset, 0.0f);

        float colatitude = latitudeMin
            + (latitudeMax - latitudeMin) * rand_xorshift_normalized(rng_state); // 3.1415
        float longitude = longitudeMin
            + (longitudeMax - longitudeMin) * rand_xorshift_normalized(rng_state_2); // 2 * 3.1415
        
        float radius = 1.0f;
        p.velocity.x = radius * cos(colatitude) * cos(longitude);
        p.velocity.z = radius * cos(colatitude) * sin(longitude);
        p.velocity.y = radius * sin(colatitude);
        p.velocity.w = 0.0f;
        
        p.velocity = normalize(mul(p.velocity, wInvTransposeMat));
        
        p.velocity = particlesBaseSpeed * p.velocity;
        // p.velocity = mul(rotMatrix, p.velocity);
        
        p.colorStart = colorStart;
        p.colorEnd = colorEnd;
        
        p.alphaStart = alphaStart;
        p.alphaEnd = alphaEnd;
        
        p.sizeStart = particleSizeStart;
        p.sizeEnd = particleSizeEnd;
        
        p.lifeSpan = particlesLifeSpan;
        p.age = abs(p.lifeSpan); //abs() so if lifetime is infinite ( < 0.0) it's still has a life

        p.mass = particlesMass;
        
        uint index = deadListBuffer.Consume();
        particleList[index] = p;

        ParticleIndexElement pe;
        pe.index = index;
        pe.distance = 0; //initialized in simulation
        aliveParticleIndex.Append(pe);

        InterlockedAdd(indirectDispatchArgs[0], 1);
    }

}
