Texture2D tex : register(t0);
SamplerState samp : register(s0);

cbuffer UUIDCBuf : register(b0)
{
    uint hi;
    uint lo;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float3 normal : NORMAL0;
    float3 wPos : POSITION;
    float2 texCoord : TEXCOORD0;
};

struct PSOutput
{
    float4 Normal : SV_Target0;
    float4 Color : SV_Target1;
    float4 WorldPos : SV_Target2;
    uint2 UUID : SV_Target3;
};

PSOutput PSMain(PS_IN input)
{   
    PSOutput output;
    output.Normal = float4(normalize(input.normal), 1.0);
    output.Color = tex.Sample(samp, input.texCoord);
    output.WorldPos = float4(input.wPos, 1.0f);
    output.UUID = uint2(hi, lo);
    return output;
}
