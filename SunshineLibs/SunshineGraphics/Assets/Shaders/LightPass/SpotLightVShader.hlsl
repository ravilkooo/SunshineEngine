struct VS_IN
{
    float3 pos : POSITION0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 wPos : POSITION;
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

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    
    output.pos = mul(float4(input.pos, 1.0), wMat);
    output.pos = output.pos.xyzw / output.pos.w;
    output.wPos = output.pos.xyz;
    output.pos = mul(output.pos, viewProjMat);
	
    return output;
}
