Texture2D NormalMap : register(t0);
Texture2D AlbedoMap : register(t1);
Texture2D SpecularMap : register(t2);
Texture2D WorldPosMap : register(t3);
SamplerState Sam : register(s0);

struct DirectionalLight
{
    float4 Diffuse;
    float4 Specular;
    float3 Position;
    float pad1;
    
    float3 Direction;
    float pad2;
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
    DirectionalLight directionalLight;
};

void calcDirectionalLight(float3 wPos, float3 normal, float3 toEye, Material mat,
    DirectionalLight dirLight,
    out float4 dl_diffuse,
    out float4 dl_spec)
{
    dl_diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    dl_spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
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
    
    float4 dl_diffuse;
    float4 dl_spec;
    
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
    
    return saturate(dl_diffuse + dl_spec);
    //return float4(1.0f, 1.0f, 0.0f, 0.5f);
}