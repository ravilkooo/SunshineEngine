struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float3 normal : NORMAL0;
    float3 wPos : POSITION;
};

struct PSOutput
{
    float4 Normal : SV_Target0;
    float4 Albedo : SV_Target1;
    float2 Specular : SV_Target2;
    float4 WorldPos : SV_Target3;
};

PSOutput PSMain(PS_IN input)
{
    PSOutput output;
    output.Normal = float4(normalize(input.normal), 1.0);
    output.Albedo = input.col;
    output.Specular = float2(
        saturate(dot(input.col.xyz, float(1).xxx).x) * 0.5,
        10);
    output.WorldPos = float4(input.wPos, 1.0f);
    return output;
}