Texture2D NormalMap : register(t0);
Texture2D AlbedoMap : register(t1);
Texture2D SpecularMap : register(t2);
Texture2D WorldPosMap : register(t3);
SamplerState Sam : register(s0);

Texture2D SkyBoxMap : register(t4);
SamplerState SkyBoxSam : register(s1);

struct SkyBox
{
    float3 Tint;
    float Power;
};

struct CameraData
{
    row_major float4x4 vMatInverse;
    row_major float4x4 pMatInverse;
    float3 camPos;
    float pad;
};

struct Material
{
    float4 Diffuse;
    float2 Specular;
};

cbuffer CameraBuffer : register(b0) // per object
{
    CameraData camData;
};

cbuffer LightBuffer : register(b1) // per frame
{
    SkyBox skyBox;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float3 normal : NORMAL0;
    float3 wPos : POSITION;
    float2 texCoord : TEXCOORD0;
};

static const float SMAP_SIZE_X = 1000.0f;
static const float SMAP_SIZE_Y = 800.0f;

float4 PSMain(PS_IN input) : SV_Target
{
    float4 sampledColor = SkyBoxMap.Sample(SkyBoxSam, input.texCoord);
    float3 delta = skyBox.Tint - sampledColor.xyz;
    float4 newColor = sampledColor + float4(delta * skyBox.Power.xxx, 0.0f);
    
    return saturate(newColor);
}