// Структура частицы в шейдере
struct Particle
{
    float3 Position;
    float Size;
    float4 Color;
    
    float3 Velocity;
    float Energy;
};

RWStructuredBuffer<Particle> particlePool : register(u0);
ConsumeStructuredBuffer<uint> deadList : register(u1);

cbuffer EmitParams : register(b0)
{
    uint numParticlesToEmit; // Количество частиц для эмиссии в этом кадре
};

[numthreads(64, 1, 1)]
void CSMain(uint3 tid : SV_DispatchThreadID)
{
    if (tid.x < numParticlesToEmit)
    { // Эмитировать только нужное количество
        uint index = deadList.Consume();
        Particle p;
        p.Position = float3(index * 0.01, 1, 0); // Начальные параметры
        p.Velocity = float3(0, 1, 0);
        p.Energy = 5.0f;
        p.Color = float4(0, 0.5, 1.0, 1.0);
        p.Size = 0.1;
        particlePool[index] = p;
    }
}