Texture2D NormalMap : register(t0);
Texture2D AlbedoMap : register(t1);
Texture2D SpecularMap : register(t2);
Texture2D WorldPosMap : register(t3);
SamplerState Sam : register(s0);

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

struct AmbientLight
{
    float3 Ambient;
    float AmbientPad;
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
    AmbientLight ambientLight;
};

void calcAmbientLight(Material mat, AmbientLight ambLight,
    out float3 al_ambient)
{
    //al_ambient = mat.Ambient * ambientLight.Ambient;
    al_ambient = mat.Diffuse * ambLight.Ambient * 0.5;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};
 
float4 PSMain(PS_IN input) : SV_Target
{
    
    float x = input.pos.x / screenInfo.sceenSize.x;
    float y = input.pos.y / screenInfo.sceenSize.y;
    Material mat =
    {
        float3(AlbedoMap.Sample(Sam, float2(x, y)).rgb),
        float2(SpecularMap.Sample(Sam, float2(x, y)).rg)
    };
    
    float3 al_ambient;
    calcAmbientLight(mat, ambientLight,
        al_ambient);
    
    return saturate(float4(al_ambient, 1.0f));
    //return float4(1.0f, 1.0f, 0.0f, 0.5f);
}