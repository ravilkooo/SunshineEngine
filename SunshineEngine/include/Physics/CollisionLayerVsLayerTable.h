#pragma once

#include <EASTL/vector.h>
#include <EASTL/unordered_set.h>
#include <EASTL/unordered_map.h>

#include <Physics/CollisionUtils.h>

class CollisionLayerVsLayerTable
{
public:

    eastl::unordered_map<
        CollisionLayer,
        eastl::unordered_set<CollisionLayer>>
        m_collisionLayerVsLayerTable;

    void DisableCollision(CollisionLayer inLayer1, CollisionLayer inLayer2);
    void EnableCollision(CollisionLayer inLayer1, CollisionLayer inLayer2);
    bool ShouldCollide(CollisionLayer inLayer1, CollisionLayer inLayer2);

};
