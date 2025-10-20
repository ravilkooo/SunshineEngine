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

struct ScreenInfo
{
    float2 sceenSize;
};

struct Material
{
    float3 Diffuse;
    float2 Specular;
};

cbuffer CameraBuffer : register(b0) // per frame
{
    CameraData camData;
};

cbuffer ScreenInfoBuffer : register(b1) // per frame
{
    ScreenInfo screenInfo;
};

cbuffer LightBuffer : register(b2) // per object
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

float4 PSMain(PS_IN input) : SV_Target
{
    float3 sampledColor = SkyBoxMap.Sample(SkyBoxSam, input.texCoord).xyz;
    float3 delta = skyBox.Tint - sampledColor;
    float3 newColor = sampledColor + delta * skyBox.Power.xxx;
    
    return saturate(float4(newColor, 1.0f));
}