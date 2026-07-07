#define M_PI 3.14159265358979323846f

struct VS_IN
{
    float3 pos : POSITION0;
    uint vertexID   : SV_VertexID;
    uint instanceID : SV_InstanceID;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

cbuffer TransformCBuf : register(b0)
{
    row_major float4x4 wMat;
    row_major float4x4 wInvTransposeMat;
    row_major float4x4 lMat;
};

cbuffer CameraCBuf : register(b1)
{
    row_major float4x4 viewProjMat;
};

cbuffer PerceptionSettings : register(b2)
{
    float3 EyesOffset;
    float padding;
    
    float SightRadius;
    float LoseRadius;
    float FieldOfView;
    float HearingRadius;

    row_major float4x4 wMatNoLocalInvTranspose;
};

static const uint segments = 32u;

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;

    float radius = 1.0f;
    // float3 offsetVector = float3(0.0f, 0.0f, 0.0f);

    float minLongtitude = -M_PI;
    float maxLongtitude = M_PI;

    float deltaAngle = 0.0f;

    if (input.instanceID == 0u)
    {
        // Eye sight sphere
        // offsetVector = mul(float4(EyesOffset, 0.0f), wInvTransposeMat);
        
        radius = SightRadius;
        minLongtitude = -(FieldOfView * 0.0174532f) * 0.5f;
        maxLongtitude = (FieldOfView * 0.0174532f) * 0.5f;

        output.col = float4(0.0f, 1.0f, 0.0f, 1.0f);
    }
    else if (input.instanceID == 1u)
    {
        // Eye lose sphere
        // offsetVector = mul(float4(EyesOffset, 0.0f), wInvTransposeMat);

        radius = LoseRadius;
        minLongtitude = -(FieldOfView * 0.0174532f) * 0.5f;
        maxLongtitude = (FieldOfView * 0.0174532f) * 0.5f;
        
        output.col = float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        // Hearing spheres
        radius = HearingRadius;

        deltaAngle = M_PI;
        output.col = float4(1.0f, 1.0f, 0.0f, 1.0f);
    }

    if (input.vertexID == 0u)
    {
        // center vertex
        output.pos = mul(float4(EyesOffset, 1.0f), lMat);
        output.pos = mul(output.pos, wMat);
        output.pos = output.pos.xyzw / output.pos.w;
        output.pos = mul(output.pos, viewProjMat);
        return output;
    }
    // else input.vertexID is in range [1, 33]

    uint long_idx = input.vertexID - 1u; // [0, 32]
    
    // long_val = -min_long + (max_long - min_long) * long_idx / 32;
    float long_val = minLongtitude + (maxLongtitude - minLongtitude) * long_idx * 0.03125f;
    
    output.pos.x = radius * sin(long_val + deltaAngle);
    output.pos.z = radius * cos(long_val + deltaAngle);
    output.pos.y = 0.0f;
    output.pos.w = 1.0f;
    
    output.pos = mul(output.pos, lMat);
    output.pos = mul(output.pos, wMat);
    output.pos = output.pos.xyzw / output.pos.w;

    float4 offsetVector = mul(float4(EyesOffset, 0.0f), wMatNoLocalInvTranspose);
    output.pos += offsetVector;
    
    output.pos = mul(output.pos, viewProjMat);
	
    return output;
}
