//Texture2D DepthMap : register(t0);
//Texture2D NormalMap : register(t0);
Texture2D AlbedoMap : register(t0);
//Texture2D LightMap : register(t2);
SamplerState Sam : register(s0);
 
struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

static const float4x4 T =
{
    0.5f, 0.0f, 0.0f, 0.0f, // row 1
    0.0f, -0.5f, 0.0f, 0.0f, // row 2
    0.0f, 0.0f, 1.0f, 0.0f, // row 3
    0.5f, 0.5f, 0.0f, 1.0f // row 4
};

static const float SMAP_SIZE_X = 1000.0f;
static const float SMAP_SIZE_Y = 800.0f;

float4 PSMain(PS_IN input) : SV_Target
{
    float4 uv = mul(float4(input.pos), T);
    uv = uv / uv.w;
    float x = input.pos.x / SMAP_SIZE_X;
    float y = input.pos.y / SMAP_SIZE_Y;
    //return float4(input.tex.x, input.tex.y, 0, 1);
    //return float4(1.0, 1.0, 0.0f, 1.0f);
    //return float4(LightMap.Sample(Sam, float2(x, y)).rgb, 1.0f);
    return float4(AlbedoMap.Sample(Sam, float2(input.tex.x, input.tex.y)).rgb, 1.0f);
    //return float4(NormalMap.Sample(Sam, float2(input.tex.x, input.tex.y)).rgb, 1.0f);
    /*
    float3 normal;
    normal.xy = NormalMap.Sample(Sam, float2(x, y)).rg;
    normal.z = sqrt(1 - x * x - y * y);
    return float4(normal, 1.0f);
    */
}