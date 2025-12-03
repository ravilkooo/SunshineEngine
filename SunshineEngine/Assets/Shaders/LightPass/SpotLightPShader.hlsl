Texture2D NormalMap : register(t0);
Texture2D AlbedoMap : register(t1);
Texture2D SpecularMap : register(t2);
Texture2D WorldPosMap : register(t3);
SamplerState Sam : register(s0);

struct SpotLight
{
    float3 Diffuse;
    float DiffusePad;
    float3 Specular;
    float SpecularPad;
    float3 Position;
    float Range;
    
    float2 Direction;
    float Spot;
    float pad;
    
    float3 Att;
    float pad2;
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
    SpotLight spotLight;
};

void calcSpotLight(float3 wPos, float3 normal, float3 toEye,
    Material mat, SpotLight spotLight,
    out float3 sl_diffuse,
    out float3 sl_spec)
{
    sl_diffuse = float3(0.0f, 0.0f, 0.0f);
    sl_spec = float3(0.0f, 0.0f, 0.0f);
    
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
    
    float3 direction = float3(
        cos(spotLight.Direction.y) * cos(spotLight.Direction.x),
        sin(spotLight.Direction.y),
        cos(spotLight.Direction.y) * sin(spotLight.Direction.x)
    );
    
    float spot = pow(max(dot(-lightVec, direction), 0.0f), spotLight.Spot);
    

    float att = spot / dot(spotLight.Att, float3(1.0f, d, d * d));
    sl_diffuse *= att;
    sl_spec *= att;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 wPos : POSITION;
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
    
    float3 sl_diffuse;
    float3 sl_spec;
    
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
    
    return saturate(float4(sl_diffuse + sl_spec, 1.0f));
    //return float4(1.0f, 1.0f, 0.0f, 0.5f);
}