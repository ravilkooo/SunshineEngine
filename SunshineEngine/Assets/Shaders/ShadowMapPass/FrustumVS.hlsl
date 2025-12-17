struct VS_IN
{
    float3 pos : POSITION0;
    float4 col : COLOR0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

cbuffer TransformCBuf : register(b0)
{
    float3 corner_mm0; float _pad_mm0;
    float3 corner_mm1; float _pad_mm1;
    float3 corner_mp0; float _pad_mp0;
    float3 corner_mp1; float _pad_mp1;
    float3 corner_pm0; float _pad_pm0;
    float3 corner_pm1; float _pad_pm1;
    float3 corner_pp0; float _pad_pp0;
    float3 corner_pp1; float _pad_pp1;
    uint idx;
}

cbuffer CameraCBuf : register(b1)
{
    row_major float4x4 viewProjMat;
}

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    
    if (input.pos.x > 0.0f)
    {
        if (input.pos.y > 0.0f)
        {
            if (input.pos.z > 0.5f)
            {
                output.pos = float4(corner_pp1, 1.0f);
            }
            else
            {
                output.pos = float4(corner_pp1, 1.0f);
            }
        }
        else
        {
            if (input.pos.z > 0.5f)
            {
                output.pos = float4(corner_pm1, 1.0f);
            }
            else
            {
                output.pos = float4(corner_pm1, 1.0f);
            }
        }
    }
    else
    {
        if (input.pos.y > 0.0f)
        {
            if (input.pos.z > 0.5f)
            {
                output.pos = float4(corner_mp1, 1.0f);
            }
            else
            {
                output.pos = float4(corner_mp1, 1.0f);
            }
        }
        else
        {
            if (input.pos.z > 0.5f)
            {
                output.pos = float4(corner_mm1, 1.0f);
            }
            else
            {
                output.pos = float4(corner_mm1, 1.0f);
            }
        }
    }
    // output.pos = (output.pos - float4((corner_mm0 + corner_pp0) * 0.5f, 0.0f)) * 0.1f;
    
    output.pos = mul(output.pos, viewProjMat);
    if (idx == 0)
        output.col = float4(1.0f, 0.0f, 0.0f, 0.99f);
    else if (idx == 1)
        output.col = float4(0.0f, 1.0f, 0.0f, 0.99f);
    else if (idx == 2)
        output.col = float4(0.0f, 0.0f, 1.0f, 0.99f);
    else // if (idx == 3)
        output.col = float4(1.0f, 1.0f, 1.0f, 0.99f);
	
    return output;
}
