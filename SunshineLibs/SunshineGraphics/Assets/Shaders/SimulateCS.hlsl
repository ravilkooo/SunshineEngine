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
AppendStructuredBuffer<uint> deadList : register(u1);
AppendStructuredBuffer<uint2> sortList : register(u2);

cbuffer CBuf : register(b0)
{
    float3 camPos;
    float deltaTime;
};

//static const float deltaTime = 0.016;
static const float3 acceleration = float3(0, 2.0, 0);

[numthreads(64, 1, 1)]
void CSMain(int3 groupThreadID : SV_GroupThreadID,
            int3 dispatchThreadID : SV_DispatchThreadID)
{
    uint index = dispatchThreadID.x; // + groupThreadID.x * 64
    Particle p = particlePool[index];
    
    if (p.Energy > 0)
    {
    // Обновление физики
        p.Velocity += acceleration * deltaTime;
        p.Position += p.Velocity * deltaTime;
        p.Energy -= deltaTime;

        if (p.Energy <= 0)
        {
            deadList.Append(index); // Возврат в Dead List
            return;
        }
        else
        {
        // Вычисление дистанции до камеры
            float distanceSq = dot(p.Position - camPos, p.Position - camPos);
            sortList.Append(uint2(index, asuint(distanceSq))); // Добавление в Sort List
        }

        particlePool[index] = p;
    }
}