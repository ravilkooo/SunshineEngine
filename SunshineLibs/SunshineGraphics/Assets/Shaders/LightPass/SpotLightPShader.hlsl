Texture2D NormalMap : register(t0);
Texture2D AlbedoMap : register(t1);
Texture2D SpecularMap : register(t2);
Texture2D WorldPosMap : register(t3);
SamplerState Sam : register(s0);

struct SpotLight
{
    float4 Diffuse;
    float4 Specular;
    float3 Position;
    float Range;
    
    float3 Direction;
    float Spot;
    
    float3 Att;
    float pad;
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
    SpotLight spotLight;
};

void calcSpotLight(float3 wPos, float3 normal, float3 toEye,
    Material mat, SpotLight spotLight,
    out float4 sl_diffuse,
    out float4 sl_spec)
{
    sl_diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    sl_spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 lightVec = spotLight.Position - wPos;
    float d = length(lightVec);
       
    if (d > spotLight.Range)
        return;
    lightVec /= d;
    float diffuseFactor = dot(lightVec, normal);
        
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.y);
        sl_diffuse = diffuseFactor * mat.Diffuse * spotLight.Diffuse;
        sl_spec = sl_diffuse * mat.Specular.x;
    }
    
    float spot = pow(max(dot(-lightVec, spotLight.Direction), 0.0f), spotLight.Spot);
    

    float att = spot / dot(spotLight.Att, float3(1.0f, d, d * d));
    sl_diffuse *= att;
    sl_spec *= att;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 wPos : POSITION;
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
    
    float4 sl_diffuse;
    float4 sl_spec;
    
    float4 normal = float4(NormalMap.Sample(Sam, float2(x, y)).rgb, 1.0f);
    /*
    float pixelDepthValue = DepthMap.Sample(Sam, float2(x, y)).r;
    float4 pixelViewPos = mul(float4(2 * x - 1, 2 * y - 1, pixelDepthValue, 1.0f),
    camData.pMatInverse);
    pixelViewPos = pixelViewPos / pixelViewPos.w;
    float4 pixelWorldPos = mul(pixelViewPos, camData.vMatInverse);
    */
    float3 pixelWorldPos = WorldPosMap.Sample(Sam, float2(x, y)).rgb;
    
    
    float3 toEye = normalize(camData.camPos - pixelWorldPos.xyz);
    //toEye = float3(1, 0, 0);
    
    calcSpotLight(pixelWorldPos.xyz, normal.xyz, toEye, mat, spotLight,
        sl_diffuse, sl_spec);
    
    return saturate(sl_diffuse + sl_spec);
    //return float4(1.0f, 1.0f, 0.0f, 0.5f);
}