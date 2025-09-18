struct Particle
{
    float3 Position;
    float Size;
    float4 Color;
    
    float3 velocity;
    float energy;
};
StructuredBuffer<Particle> Particles : register(t4);

cbuffer CBuf
{
    row_major float4x4 WorldViewProj;
};

struct VSInput
{
    uint VertexID : SV_VertexID;
    uint InstanceID : SV_InstanceID;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    Particle p = Particles[input.InstanceID];
    // как отрисовывать только живые частицы?
    VSOutput output;
    if (p.energy < 0)
    {
        output.Position = float4(0, 0, -1, 1);
        return output;
    }
    // Создаем квад из VertexID
    output.UV = float2(
        (input.VertexID % 3) & 1,
        ((input.VertexID % 3) & 2) >> 1
    );
    if (input.VertexID / 3 > 0) 
        output.UV = 1 - output.UV;
    
    // 0, 0
    // 1, 0
    // 0, 1
    // 3
    // 12
    
    // 21
    //  3
    
    output.Position = float4(output.UV * float2(2, -2) + float2(-1, 1), 0, 1);
    output.Position = mul(float4(output.Position.xyz * p.Size + p.Position, 1.0),
    WorldViewProj);
    output.Color = p.Color;
    
    return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
    return input.Color;
    // * texture.Sample(sampler, input.UV);
}