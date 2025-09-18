// BitonicSortCS.hlsl
RWStructuredBuffer<uint2> sortList : register(u0);

groupshared uint2 sharedData[1024]; // Групповая память для сортировки

[numthreads(1024, 1, 1)]
void main(uint3 tid : SV_DispatchThreadID, uint gidx : SV_GroupIndex)
{
    // Загрузка данных в групповую память
    sharedData[gidx] = sortList[tid.x];
    GroupMemoryBarrierWithGroupSync();

    // Алгоритм битонической сортировки
    for (uint k = 2; k <= 1024; k *= 2)
    {
        for (uint j = k / 2; j > 0; j /= 2)
        {
            uint ixj = gidx ^ j;
            if (ixj > gidx)
            {
                if ((gidx & k) == 0)
                {
                    if (sharedData[gidx].y > sharedData[ixj].y)
                    {
                        swap(sharedData[gidx], sharedData[ixj]);
                    }
                }
                else
                {
                    if (sharedData[gidx].y < sharedData[ixj].y)
                    {
                        swap(sharedData[gidx], sharedData[ixj]);
                    }
                }
            }
            GroupMemoryBarrierWithGroupSync();
        }
    }

    // Сохранение результата
    sortList[tid.x] = sharedData[gidx];
}