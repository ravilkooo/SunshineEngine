#include <Physics/CollisionLayerVsGroupTable.h>

void CollisionLayerVsGroupTable::DisableCollision(CollisionLayer inLayer1, CollisionGroup inGroup2)
{
    m_collisionLayerVsGroupTable[inLayer1].erase(inGroup2);
}

void CollisionLayerVsGroupTable::EnableCollision(CollisionLayer inLayer1, CollisionGroup inGroup2)
{
    m_collisionLayerVsGroupTable[inLayer1].insert(inGroup2);
}

bool CollisionLayerVsGroupTable::ShouldCollide(CollisionLayer inLayer1, CollisionGroup inGroup2)
{
    return m_collisionLayerVsGroupTable[inLayer1].contains(inGroup2);
}