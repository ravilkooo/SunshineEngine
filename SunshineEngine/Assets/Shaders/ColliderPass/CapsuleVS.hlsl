struct VS_IN
{
    float3 pos : POSITION0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
};

cbuffer TransformCBuf : register(b0)
{
    row_major float4x4 wMat;
    row_major float4x4 wInvTransposeMat;
}

cbuffer CameraCBuf : register(b1)
{
    row_major float4x4 viewProjMat;
}

cbuffer ColliderTransformCBuf : register(b2)
{
    row_major float4x4 colliderTransformMat;
}

cbuffer TaperedCapsuleColliderSettings : register(b3)
{
    float m_height;
    float m_radius;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    output.pos = float4(input.pos, 1.0);
    
    // Collider settings
    if (output.pos.y > 0)
    {
        output.pos.y = output.pos.y - 0.5f;
        output.pos.xyz = output.pos.xyz * m_radius;
        output.pos.y += m_height * 0.5f;
    }
    else
    {
        output.pos.y = output.pos.y + 0.5f;
        output.pos.xyz = output.pos.xyz * m_radius;
        output.pos.y -= m_height * 0.5f;
    }
    
    // Collider transforms
    output.pos = mul(output.pos, colliderTransformMat);
    
    // Parent body transforms
    output.pos = mul(output.pos, wMat);
    output.pos = output.pos.xyzw / output.pos.w;
    output.pos = mul(output.pos, viewProjMat);
	
    return output;
}
