
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

    float screenSpin;
    float screenSpinSpeed;
    float worldSpin;
    float worldSpinSpeed;

    float3 worldSpinAxis;
    float mass;
    
    float orientaion;
    float3 padding;
};

struct ParticleIndexElement
{
    float distance;
    float index;
};
