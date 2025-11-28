Texture2D NormalMap : register(t0);
Texture2D AlbedoMap : register(t1);
Texture2D SpecularMap : register(t2);
Texture2D WorldPosMap : register(t3);
SamplerState Sam : register(s0);

struct DirectionalLight
{
    float3 Diffuse;
    float DiffusePad;
    float3 Specular;
    float SpecularPad;
    float3 Position;
    float pad1;
    
    float3 Direction;
    float pad2;
};

struct CameraData
{
    row_major float4x4 viewMat;
    row_major float4x4 projMat;
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

struct ShadowTransforms
{
    row_major float4x4 lightView;
    row_major float4x4 lightProj;
    row_major float4x4 shadowTransform;
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
    DirectionalLight directionalLight;
};

cbuffer CascadeCBuf : register(b3) // per frame
{
    ShadowTransforms shTransforms[4];
    float4 distances;
};

Texture2DArray shadowMap : register(t4);
SamplerState shadowSampler : register(s1);
SamplerComparisonState samShadow : register(s2)
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    BorderColor = float4(1.0f, 1.0f, 1.0f, 0.0f);

    ComparisonFunc = LESS;
};

void calcDirectionalLight(float3 wPos, float3 normal, float3 toEye, Material mat,
    DirectionalLight dirLight,
    out float3 dl_diffuse,
    out float3 dl_spec)
{
    dl_diffuse = float3(0.0f, 0.0f, 0.0f);
    dl_spec = float3(0.0f, 0.0f, 0.0f);
    
    {
        float3 lightVec = -dirLight.Direction;
        float diffuseFactor = dot(lightVec, normal);
        
        [flatten]
        if (diffuseFactor > 0.0f)
        {
            float3 v = reflect(-lightVec, normal);
            float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.y);
            dl_diffuse = diffuseFactor * mat.Diffuse * dirLight.Diffuse;
            dl_spec = dl_diffuse * mat.Specular.x;
        }
    }
}

float CalcShadowFactor(SamplerComparisonState samShadow,
                       Texture2DArray shadowMap,
                       float4 shadowPosH,
                       int layer)
{
  // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;
  
  // Depth in NDC space.
    float depth = shadowPosH.z;

    // if (samShadow.Sample())
  
    //return shadowMap.SampleCmpLevelZero(samShadow, shadowPosH.xy, depth).r;
  
    // Texel size.
    const float dx = 1.0f / screenInfo.sceenSize.x;
    const float dy = 1.0f / screenInfo.sceenSize.y;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dy), float2(0.0f, -dy), float2(dx, -dy),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dy), float2(0.0f, +dy), float2(dx, +dy)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(
            samShadow,
            float3(shadowPosH.xy + offsets[i], layer),
            depth).r;
    }

    return percentLit /= 9.0f;
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
    
    float3 dl_diffuse;
    float3 dl_spec;
    
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
    
    calcDirectionalLight(pixelWorldPos.xyz, normal.xyz, toEye, mat, directionalLight,
        dl_diffuse, dl_spec);
    
    // ---------------------------------------------
    
    int layer = 1;
    
    static float cascadeDistances[4] = (float[4]) distances;
    
    float4 viewPos = mul(float4(pixelWorldPos, 1), camData.viewMat);
    viewPos /= viewPos.w;
    
    for (int i = 0; i < 4; ++i)
    {
        //if (lightViewPos.z < cascadeDistances[i])
        if (viewPos.z < cascadeDistances[i])
        {
            layer = i;
            break;
        }
    }
    
    float4 shPos = mul(float4(pixelWorldPos, 1.0), shTransforms[layer].shadowTransform);
    shPos = shPos / shPos.w;
    
    float4 shadowPictureColor = float4(0, 0, 0, 0);
    float shadowFactor = 0;
    
    float3 dirLightCol;
    if ((shPos.x >= 0) && (shPos.y >= 0) && (shPos.z >= 0) && (shPos.x <= 1) && (shPos.y <= 1) && (shPos.z <= 1))
    {
        shadowFactor = CalcShadowFactor(samShadow, shadowMap, shPos, layer);
        
        dirLightCol = saturate(shadowFactor * (dl_diffuse + dl_spec));
    }
    else
        dirLightCol = saturate(dl_spec + dl_diffuse);

    // ---------------------------------------------
    
    return saturate(float4(dirLightCol, 1.0f));
}