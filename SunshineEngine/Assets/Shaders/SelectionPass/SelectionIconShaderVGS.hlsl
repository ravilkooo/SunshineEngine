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
}

GS_IN VSMain(VS_IN input)
{
    GS_IN output = (GS_IN) 0;
    
    output.pos = mul(float4(input.pos, 1.0), wMat);
    output.pos = output.pos.xyzw / output.pos.w;
    output.size = input.size;

    return output;
}

struct CameraData
{
    row_major float4x4 viewProjMat;
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
    float4 pos = input[0].pos;
    float width = input[0].size.x * 10.0f * scaleFactor;
    float height = input[0].size.y * 10.0f * scaleFactor;
    
	// Calculate vector perpendicular to the camera
    float3 plane_normal = pos.xyz - camData.camPos;
    // plane_normal.y = 0.0f;
    plane_normal = normalize(plane_normal);
    // plane_normal = float3(0.0f, 0.0f, 1.0f);

    float3 up_vector = float3(0.0f, 1.0f, 0.0f);
    float3 right_vector = normalize(cross(plane_normal, up_vector));
    // right_vector = float3(1.0f, 0.0f, 0.0f);

	// Create rectangle vertices that face the camera
    float3 vertices[4];
    vertices[0] = pos.xyz + width * right_vector - height * up_vector;
    vertices[1] = pos.xyz + width * right_vector + height * up_vector;
    vertices[2] = pos.xyz - width * right_vector - height * up_vector;
    vertices[3] = pos.xyz - width * right_vector + height * up_vector;
    
	// Append output stream with our 4 new rectangle vertices
	[unroll]
    for (uint i = 0; i < 4; i++)
    {
        PS_IN element;

        element.pos = float4(vertices[i], 1.0f);
        element.pos = mul(element.pos, camData.viewProjMat);
        
        output.Append(element);
    }
}
