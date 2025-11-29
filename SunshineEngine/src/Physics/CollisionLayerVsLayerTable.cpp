#include <Physics/CollisionLayerVsLayerTable.h>

namespace SE
{
    void CollisionLayerVsLayerTable::DisableCollision(CollisionLayer inLayer1, CollisionLayer inLayer2)
    {
        m_collisionLayerVsLayerTable[inLayer1].erase(inLayer2);
        m_collisionLayerVsLayerTable[inLayer2].erase(inLayer1);
    }

    void CollisionLayerVsLayerTable::EnableCollision(CollisionLayer inLayer1, CollisionLayer inLayer2)
    {
        m_collisionLayerVsLayerTable[inLayer1].insert(inLayer2);
        m_collisionLayerVsLayerTable[inLayer2].insert(inLayer1);
    }

    bool CollisionLayerVsLayerTable::ShouldCollide(CollisionLayer inLayer1, CollisionLayer inLayer2)
    {
        return m_collisionLayerVsLayerTable[inLayer1].contains(inLayer2);
    }
}