struct VS_IN
{
    float3 pos : POSITION;
    float2 size : SIZE;
};

struct GS_IN
{
    float4 pos : POSITION;
    float2 size : SIZE;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
};

cbuffer TransformCBuf : register(b0)
{
    row_major float4x4 wMat;
    row_major float4x4 wInvTransposeMat;
    row_major float4x4 lMat;
}

GS_IN VSMain(VS_IN input)
{
    GS_IN output = (GS_IN) 0;
    
    output.pos = mul(float4(input.pos, 1.0), lMat);
    output.pos = mul(output.pos, wMat);
    output.pos = output.pos.xyzw / output.pos.w;
    output.size = input.size;

    return output;
}

struct CameraData
{
    row_major float4x4 viewMat;
    row_major float4x4 projMat;
    float3 camPos;
    float pad;
};

cbuffer CameraBuffer : register(b0) // per frame
{
    CameraData camData;
};

cbuffer SelectionBuffer : register(b1) // per frame
{
    float scaleFactor;
};

[maxvertexcount(4)]
void GSMain(point GS_IN input[1], inout TriangleStream<PS_IN> output)
{
	// Shorten variables
    float4 viewPos = mul(float4(input[0].pos.xyz, 1.0f), camData.viewMat);
    float width = 0.5f * input[0].size.x * 10.0f * scaleFactor;
    float height = 0.5f * input[0].size.y * 10.0f * scaleFactor;

	// Create rectangle vertices that face the camera
    float3 up_vector = float3(0.0f, 1.0f, 0.0f);
    float3 right_vector = float3(1.0f, 0.0f, 0.0f);
    
    float3 vertices[4];
    vertices[0] = -width * right_vector - height * up_vector;
    vertices[1] = -width * right_vector + height * up_vector;
    vertices[2] = +width * right_vector - height * up_vector;
    vertices[3] = +width * right_vector + height * up_vector;
    
    // UV
    float2 uv[4];
    uv[0] = float2(0.0f, 1.0f);
    uv[1] = float2(0.0f, 0.0f);
    uv[2] = float2(1.0f, 1.0f);
    uv[3] = float2(1.0f, 0.0f);
    
	// Append output stream with our 4 new rectangle vertices
	[unroll]
    for (uint i = 0; i < 4; i++)
    {
        PS_IN element;

        element.pos = float4(viewPos.xyz + vertices[i], 1.0f);
        element.pos = mul(element.pos, camData.projMat);
        
        output.Append(element);
    }
}
