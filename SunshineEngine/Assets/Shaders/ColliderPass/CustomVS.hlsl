struct VS_IN
{
    float3 pos : POSITION0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

cbuffer TransformCBuf : register(b0)
{
    row_major float4x4 wMat;
    row_major float4x4 wInvTransposeMat;
    row_major float4x4 lMat;
}

cbuffer CameraCBuf : register(b1)
{
    row_major float4x4 viewProjMat;
}

cbuffer ColliderTransformCBuf : register(b2)
{
    row_major float4x4 colliderTransformMat;
}

cbuffer BoxColliderSettings : register(b3)
{
    float4 colliderColor;
    float3 m_size;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    output.col = colliderColor;
    output.pos = float4(input.pos, 1.0);
    
    // Collider settings
    output.pos.xyz = output.pos.xyz * m_size;
    
    // Collider transforms
    output.pos = mul(output.pos, colliderTransformMat);
    
    // Parent body transforms
    output.pos = mul(mul(output.pos, lMat), wMat);
    output.pos = output.pos.xyzw / output.pos.w;
    output.pos = mul(output.pos, viewProjMat);
	
    return output;
}
