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

cbuffer MapSizeBuffer : register(b4) // per frame
{
    ScreenInfo shadowMapSize;
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
    if (shadowPosH.x < 0 || shadowPosH.y < 0 || shadowPosH.z < 0
        || shadowPosH.x > 1 || shadowPosH.y > 1 || shadowPosH.z > 1)
    {
        return 1.0f;
    }
  
    // Depth in NDC space.
    float depth = shadowPosH.z;
    
    // Base texel size in UV space
    float2 texelSize = 1.0f / shadowMapSize.sceenSize;

    // Optional: scale kernel radius (e.g. softer shadows for larger cascades)
    float kernelScale = 1.0f; // tweak per cascade if needed
    texelSize *= kernelScale;
    
    const int KERNEL_HALF = 2; // 5x5 kernel
    float sum = 0.0f;
    int count = 0;

    [unroll]
    for (int dy = -KERNEL_HALF; dy <= KERNEL_HALF; ++dy)
    {
        [unroll]
        for (int dx = -KERNEL_HALF; dx <= KERNEL_HALF; ++dx)
        {
            float2 uv = shadowPosH.xy + float2(dx, dy) * texelSize;

            // Optional clamp to avoid sampling BORDER color
            uv = saturate(uv);

            sum += shadowMap.SampleCmpLevelZero(
                       samShadow,
                       float3(uv, layer),
                       depth).r;
            ++count;
        }
    }

    return sum / count;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

static float cascadeDistances[4] = (float[4]) distances;

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
    float3 pixelWorldPos = WorldPosMap.Sample(Sam, float2(x, y)).rgb;
    float3 toEye = normalize(camData.camPos - pixelWorldPos.xyz);
    
    calcDirectionalLight(pixelWorldPos.xyz, normal.xyz, toEye, mat, directionalLight,
        dl_diffuse, dl_spec);
    
    // ---------------------------------------------
    
    int layer = 1;
    
    float4 viewPos = mul(float4(pixelWorldPos, 1), camData.viewMat);
    viewPos /= viewPos.w;
    
    for (int i = 0; i < 4; ++i)
    {
        if (viewPos.z < cascadeDistances[i])
        {
            layer = i;
            break;
        }
    }
    
    float4 shPos = mul(float4(pixelWorldPos, 1.0), shTransforms[layer].shadowTransform);
    shPos = shPos / shPos.w;
    
    float shadowFactor = CalcShadowFactor(samShadow, shadowMap, shPos, layer);
    
    float3 dirLightCol = saturate(shadowFactor * (dl_diffuse + dl_spec));

    // ---------------------------------------------
    
    return saturate(float4(dirLightCol, 1.0f));
}