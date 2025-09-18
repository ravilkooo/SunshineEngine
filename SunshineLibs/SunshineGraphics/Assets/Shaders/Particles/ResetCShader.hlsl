#include "ParticlesGlobals.h"

AppendStructuredBuffer<uint> deadListBuffer : register(u0);
RWStructuredBuffer<Particle> particleList : register(u1);
RWBuffer<uint> indirectDispatchArgs : register(u2);

[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    //first thread to initialise the arguments
    if (id.x == 0)
    {
        indirectDispatchArgs[0] = 0;
        indirectDispatchArgs[1] = 1;
        indirectDispatchArgs[2] = 1;
    }

    // Wait after draw args are written so no other threads can write to them before they are initialized
    GroupMemoryBarrierWithGroupSync();

    deadListBuffer.Append(id.x);

    particleList[id.x] = (Particle) 0;

}