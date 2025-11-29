#pragma once

#include <EASTL/vector.h>
#include <EASTL/unordered_set.h>
#include <EASTL/unordered_map.h>

#include <Physics/CollisionUtils.h>

namespace SE
{
    class CollisionLayerVsGroupTable
    {
    public:
        eastl::unordered_map<
            CollisionLayer,
            eastl::unordered_set<CollisionGroup>>
            m_collisionLayerVsGroupTable;

        void DisableCollision(CollisionLayer inLayer1, CollisionGroup inGroup2);
        void EnableCollision(CollisionLayer inLayer1, CollisionGroup inGroup2);
        bool ShouldCollide(CollisionLayer inLayer1, CollisionGroup inGroup2);

    };
}