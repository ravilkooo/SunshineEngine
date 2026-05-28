#pragma once

#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>
#include <EASTL/unique_ptr.h>

#include <unordered_map>

#include <Utils/UUID.h>

class GameObject_Info;

struct SceneNode {
    SE::UUID objUUID = SE::UUID(0u);
    SE::UUID parent = SE::UUID(0u);
    eastl::vector<SE::UUID> children;
};

class SceneGraph
{
public:
    eastl::vector<SceneNode> m_nodes;                       // storage
    eastl::vector<SE::UUID> m_roots;                        // top-level node UUIDs
    eastl::unordered_map<SE::UUID, int> m_byObjUUID;        // UUID -> index mapping

    std::unordered_map<SE::UUID, eastl::unique_ptr<GameObject_Info>>& m_uuidToObjectMap; // fast lookup for GameObjects

    SceneGraph(std::unordered_map<SE::UUID, eastl::unique_ptr<GameObject_Info>>& uuidToObjectMap);
    ~SceneGraph();
    
    void Clear();
    void Build();

    void Attach(SE::UUID childUUID, SE::UUID newParentUUID);

    SE::UUID Add(SE::UUID objUUID);

    void Reparent(SE::UUID objUUID, SE::UUID newParent);

    void EraseSubtree(SE::UUID nodeUUID);
};
