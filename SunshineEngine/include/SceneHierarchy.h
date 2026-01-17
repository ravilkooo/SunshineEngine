#pragma once

#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>

#include <GameObject/GameObject.h>
#include <GameObject/ParentNode.h>

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

    SceneGraph(std::unordered_map<SE::UUID, eastl::unique_ptr<GameObject_Info>>& uuidToObjectMap)
        : m_uuidToObjectMap(uuidToObjectMap)
    {
    }
    
    ~SceneGraph()
    {
        Clear();
    }
    
    void Clear()
    {
        m_nodes.clear();
        m_roots.clear();
        m_byObjUUID.clear();
        // m_uuidToObjectMap.clear();
    }

    void Build() {
        m_nodes.clear(); m_roots.clear(); m_byObjUUID.clear();
        m_nodes.reserve(m_uuidToObjectMap.size());
        // Filling nodes storage
        for (auto& o : m_uuidToObjectMap) { m_nodes.push_back(SceneNode{ o.first, SE::UUID(0u), {} }); }

        // Filling lookup map for SceneNodes
        for (int i = 0; i < m_nodes.size(); ++i) m_byObjUUID[m_nodes[i].objUUID] = i;

        // Filling root nodes vector and save parents
        for (auto& n : m_nodes) {
            GameObject_Info* p = m_uuidToObjectMap.at(n.objUUID)->m_parent.ptr;
            if (p) {
                auto it = m_byObjUUID.find(p->m_UUID);
                if (it != m_byObjUUID.end()) {
                    n.parent = p->m_UUID;
                    m_nodes[it->second].children.push_back(n.objUUID);
                }
                else {
                    // parent not present => treat as root
                    m_roots.push_back(n.objUUID);
                }
            }
            else {
                m_roots.push_back(n.objUUID);
            }
        }
    }

    void Attach(SE::UUID childUUID, SE::UUID newParentUUID) {
        auto childIt = m_byObjUUID.find(childUUID);
        if (childIt == m_byObjUUID.end()) return;
        SceneNode& child = m_nodes[childIt->second];

        // detach from old parent/roots
        if (child.parent != SE::UUID(0u)) {
            auto parentIt = m_byObjUUID.find(child.parent);
            if (parentIt != m_byObjUUID.end()) {
                auto& siblings = m_nodes[parentIt->second].children;
                siblings.erase(eastl::remove(siblings.begin(), siblings.end(), childUUID), siblings.end());
            }
        }
        else {
            m_roots.erase(eastl::remove(m_roots.begin(), m_roots.end(), childUUID), m_roots.end());
        }
        // attach to new parent or roots
        child.parent = newParentUUID;
        if (newParentUUID != SE::UUID(0u)) {
            auto newParentIt = m_byObjUUID.find(newParentUUID);
            if (newParentIt != m_byObjUUID.end()) {
                m_nodes[newParentIt->second].children.push_back(childUUID);
            }
        }
        else {
            m_roots.push_back(childUUID);
        }
    }

    SE::UUID Add(SE::UUID objUUID)
    {
        m_nodes.push_back(SceneNode{ objUUID, SE::UUID(0u), {} });
        m_byObjUUID[objUUID] = m_nodes.size() - 1;
        SE::UUID parentUUID = SE::UUID(0u);
        if (m_uuidToObjectMap[objUUID]->m_parent.ptr) {
            parentUUID = m_uuidToObjectMap[objUUID]->m_parent.ptr->m_UUID;
        }
        Attach(objUUID, parentUUID);
        return objUUID;
    }

    void Reparent(SE::UUID objUUID, SE::UUID newParent) {
        Attach(objUUID, newParent);
    }

    void EraseSubtree(SE::UUID nodeUUID) {
        auto nodeIt = m_byObjUUID.find(nodeUUID);
        if (nodeIt == m_byObjUUID.end()) return;
        SceneNode& n = m_nodes[nodeIt->second];

        // remove mapping for subtree first
        eastl::vector<SE::UUID> stack{ nodeUUID };
        while (!stack.empty()) {
            SE::UUID cur = stack.back(); stack.pop_back();
            auto curIt = m_byObjUUID.find(cur);
            if (curIt != m_byObjUUID.end()) {
                for (auto childUUID : m_nodes[curIt->second].children) stack.push_back(childUUID);
                m_byObjUUID.erase(cur);
            }
        }
        // detach from parent/roots
        if (n.parent != SE::UUID(0u)) {
            auto parentIt = m_byObjUUID.find(n.parent);
            if (parentIt != m_byObjUUID.end()) {
                auto& siblings = m_nodes[parentIt->second].children;
                siblings.erase(eastl::remove(siblings.begin(), siblings.end(), nodeUUID), siblings.end());
            }
        }
        else {
            m_roots.erase(eastl::remove(m_roots.begin(), m_roots.end(), nodeUUID), m_roots.end());
        }
    }
};
