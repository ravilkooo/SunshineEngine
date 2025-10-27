Texture2D<uint2> UUIDTexture : register(t0);

cbuffer MouseClickPosition : register(b0)
{
    uint x;
    uint y;
    uint2 pad;
}

RWBuffer<uint> OutputUUID : register(u0);

[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    OutputUUID[0] = UUIDTexture.Load(int3(x,y,0)).x;
    OutputUUID[1] = UUIDTexture.Load(int3(x,y,0)).y;
}
