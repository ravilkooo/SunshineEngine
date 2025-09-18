Texture2D NormalMap : register(t0);
Texture2D AlbedoMap : register(t1);
Texture2D SpecularMap : register(t2);
Texture2D WorldPosMap : register(t3);
SamplerState Sam : register(s0);

struct AmbientLight
{
    float4 Ambient;
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
    AmbientLight ambientLight;
};

void calcAmbientLight(Material mat, AmbientLight ambLight,
    out float4 al_ambient)
{
    //al_ambient = mat.Ambient * ambientLight.Ambient;
    al_ambient = mat.Diffuse * ambLight.Ambient * 0.5;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

static const float SMAP_SIZE_X = 1000.0f;
static const float SMAP_SIZE_Y = 800.0f;

float4 PSMain(PS_IN input) : SV_Target
{
    
    float x = input.pos.x / SMAP_SIZE_X;
    float y = input.pos.y / SMAP_SIZE_Y;
    Material mat =
    {
        float4(AlbedoMap.Sample(Sam, float2(x, y)).rgb, 1.0f),
        float2(SpecularMap.Sample(Sam, float2(x, y)).rg)
    };
    
    float4 al_ambient;
    calcAmbientLight(mat, ambientLight,
        al_ambient);
    
    return saturate(al_ambient);
    //return float4(1.0f, 1.0f, 0.0f, 0.5f);
}