Texture2D<uint2> UUIDTexture : register(t0);
Texture2D WorldPosMap : register(t1);
Texture2D NormalMap : register(t2);

cbuffer MouseClickPosition : register(b0)
{
    uint x;
    uint y;
    uint2 pad;
}

struct PixelInfo
{
    float3 worldPos;
    uint hi;
    float3 worldNormal;
    uint lo;
};

RWStructuredBuffer<PixelInfo> OutputUUID : register(u0);

[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    OutputUUID[0].worldPos = WorldPosMap.Load(int3(x, y, 0)).xyz;
    OutputUUID[0].worldNormal = NormalMap.Load(int3(x, y, 0)).xyz;
    OutputUUID[0].hi = UUIDTexture.Load(int3(x,y,0)).x;
    OutputUUID[0].lo = UUIDTexture.Load(int3(x,y,0)).y;
}
