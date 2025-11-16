struct VS_IN
{
    float3 pos : POSITION;
    float2 size : SIZE;
};

struct GS_IN
{
    float4 pos : POSITION;
    float2 size : SIZE;
};

cbuffer TransformCBuf : register(b0)
{
    row_major float4x4 wMat;
    row_major float4x4 wInvTransposeMat;
}

GS_IN VSMain(VS_IN input)
{
    GS_IN output = (GS_IN) 0;
    
    output.pos = mul(float4(input.pos, 1.0), wMat);
    output.pos = output.pos.xyzw / output.pos.w;
    output.size = input.size;

    return output;
}
