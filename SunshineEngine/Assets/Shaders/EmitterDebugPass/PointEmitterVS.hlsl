struct VS_IN
{
    float3 pos : POSITION0;
    uint InstanceId : SV_InstanceID;
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

cbuffer PointEmitterSettings : register(b2)
{
	float minLongtitude;
	float maxLongtitude;
    float minLatitude;
    float maxLatitude;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
    output.pos = float4(input.pos, 1.0);
    
    // Emmitter settings
    uint lat_idx = input.InstanceId / 17;
    uint long_idx = input.InstanceId % 17;
    // lat_val = -min_lat + (max_lat - min_lat) * lat_idx / 16;
    float lat_val = minLatitude + (maxLatitude - minLatitude) * lat_idx * 0.125;
    // long_val = -min_long + (max_long - min_long) * long_idx / 8;
    float long_val = minLongtitude + (maxLongtitude - minLongtitude) * long_idx * 0.0625;

    output.pos.x = input.pos.x * cos(lat_val) * cos(long_val);
    output.pos.z = input.pos.x * cos(lat_val) * sin(long_val);
    output.pos.y = input.pos.x * sin(lat_val);
    
    // Parent body transforms
    output.pos = mul(output.pos, wMat);
    output.pos = output.pos.xyzw / output.pos.w;
    output.pos = mul(output.pos, viewProjMat);
	
    return output;
}
