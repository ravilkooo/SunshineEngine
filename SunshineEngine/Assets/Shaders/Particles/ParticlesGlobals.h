
struct Particle
{
    float4 position;
    float4 velocity;

    float3 colorStart;
    float alphaStart;

    float3 colorEnd;
    float alphaEnd;

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
