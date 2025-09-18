#include "ParticlesGlobals.h"

struct GeometryShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float size : TEXCOORD1;
    float4 Color : TEXCOORD2;
    float3 velocity : TEXCOORD3;
    //float4 Normal : TEXCOORD2;
    //float4 uvSprite : TEXCOORD6; //x,y for x,y and zw for size
    //uint   orientation : TEXCOORD4;
};
struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float3 center : TEXCOORD2;
    float radius : TEXCOORD3;
    float2 uv : TEXCOORD4;
    //float4 Normal : TEXCOORD2;
};

cbuffer sceneConstantBuffer : register(b0)
{
    row_major float4x4 view;
    row_major float4x4 proj;
    row_major float4x4 viewProj;
}

[maxvertexcount(4)]
void main(point GeometryShaderInput input[1], inout TriangleStream<PixelShaderInput> OutStream)
{
    //discard if no particle
    if (input[0].Color.a <= 0.0)
        return;

    PixelShaderInput output;
    output.Color = input[0].Color;
    //output.Normal = input[0].Normal;

    float3 pos = input[0].oPosition;
    float particleSize = input[0].size;

    output.center = input[0].oPosition;
    output.radius = particleSize;

    float3 right = float3(1.0, 0.0, 0.0);
    float3 up = float3(0.0, 1.0, 0.0);
    
    // if (input[0].orientation == PARTICLE_ORIENTATION_BILLBOARD)
    {
        //Camera Plane
        right = view._m00_m10_m20;
        up = view._m01_m11_m21;
    }

    // float4x4 viewProj = mul(view, proj);

    //in counterClockwise and right handed coordinate
    // 1    3
    // |  / |
    // | /  |
    // 2    4

    // Upper left vertex
    output.oPosition = pos + particleSize * (-right + up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    //output.uv = input[0].uvSprite.xy;
    output.uv = 0;
    OutStream.Append(output);

    // Bottom left vertex
    output.oPosition = pos + particleSize * (-right - up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    //output.uv = float2(input[0].uvSprite.x, input[0].uvSprite.y + input[0].uvSprite.w);
    output.uv = float2(0, 1);
    OutStream.Append(output);

    // Upper right vertex
    output.oPosition = pos + particleSize * (right + up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    //output.uv = float2(input[0].uvSprite.x + input[0].uvSprite.z, input[0].uvSprite.y);
    output.uv = float2(1, 0);
    OutStream.Append(output);

    // Bottom right vertex
    output.oPosition = pos + particleSize * (right - up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    //output.uv = input[0].uvSprite.xy + input[0].uvSprite.zw;
    output.uv = float2(1, 1);
    OutStream.Append(output);

    OutStream.RestartStrip();

}