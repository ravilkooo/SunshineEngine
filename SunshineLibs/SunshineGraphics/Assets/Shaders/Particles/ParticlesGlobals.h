
struct Particle
{
    float4 position;
    float4 velocity;
    float4 colorStart;
    float4 colorEnd;

    float sizeStart;
    float sizeEnd;
    float age;
    float lifeSpan;

    float mass;
    float3 padding;
};

struct ParticleIndexElement
{
    float distance;
    float index;
};
