struct VS_IN
{
    float3 pos : POSITION0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 wPos : POSITION;
};

cbuffer CBuf
{
    row_major float4x4 wMat;
    row_major float4x4 wMatInvTranspose;
    row_major float4x4 vpMat;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    
    output.pos = mul(float4(input.pos, 1.0), wMat);
    output.pos = output.pos.xyzw / output.pos.w;
    output.wPos = output.pos.xyz;
    output.pos = mul(output.pos, vpMat);
    //output.col = input.col;
        
    //output.normal = normalize(mul(float4(input.normal, 0), wMatInvTranspose));
	
    return output;
}
