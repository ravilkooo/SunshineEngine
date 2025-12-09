#pragma once

#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>

#include <GameObject/GameObject.h>
#include <GameObject/ParentNode.h>

struct SceneNode {
    SE::UUID objUUID = SE::UUID(0u);    // non-owning
    SceneNode* parent = nullptr;        // non-owning
    eastl::vector<SceneNode*> children;
};

class SceneGraph
{
public:
    eastl::vector<SceneNode> m_nodes;                       // storage
    eastl::vector<SceneNode*> m_roots;                      // top-level nodes
    eastl::unordered_map<SE::UUID, SceneNode*> m_byObjUUID; // fast lookup for SceneNodes

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
        m_nodes.reserve(m_uuidToObjectMap.size()); // keep pointers stable
        // Filling nodes storage
        for (auto& o : m_uuidToObjectMap) { m_nodes.push_back(SceneNode{ o.first, nullptr, {} }); }

        // Filling lookup map for SceneNodes
        for (auto& n : m_nodes) m_byObjUUID[n.objUUID] = &n;

        // Filling root nodes vector and save parentnes
        for (auto& n : m_nodes) {
            GameObject_Info* p = m_uuidToObjectMap.at(n.objUUID)->m_parent.ptr;
            if (p) {
                auto it = m_byObjUUID.find(p->m_UUID);
                if (it != m_byObjUUID.end()) {
                    n.parent = it->second;
                    it->second->children.push_back(&n);
                }
                else {
                    // parent not present => treat as root
                    // Not possible?
                    m_roots.push_back(&n);
                }
            }
            else {
                m_roots.push_back(&n);
            }
        }
    }

    void Attach(SceneNode* child, SceneNode* newParent) {
        // detach from old parent/roots
        if (child->parent) {
            auto& siblings = child->parent->children;
            siblings.erase(eastl::remove(siblings.begin(), siblings.end(), child), siblings.end());
        }
        else {
            m_roots.erase(eastl::remove(m_roots.begin(), m_roots.end(), child), m_roots.end());
        }
        // attach to new parent or roots
        child->parent = newParent;
        if (newParent) newParent->children.push_back(child);
        else m_roots.push_back(child);
    }

    SceneNode* Add(SE::UUID objUUID)
    {
        m_nodes.push_back(SceneNode{ objUUID, nullptr, {} });
        SceneNode* n = &m_nodes.back();
        m_byObjUUID[objUUID] = n;
        Attach(n, m_uuidToObjectMap[objUUID]->m_parent.ptr ?
            m_byObjUUID[m_uuidToObjectMap[objUUID]->m_parent.ptr->m_UUID] : nullptr);

        return n;
    }

    void Reparent(SE::UUID objUUID, SE::UUID newParent) {
        SceneNode* n = m_byObjUUID[objUUID];
        SceneNode* p = newParent ? m_byObjUUID[newParent] : nullptr;
        Attach(n, p);
    }

    void EraseSubtree(SceneNode* n) {
        // remove mapping for subtree first
        eastl::vector<SceneNode*> stack{ n };
        while (!stack.empty()) {
            SceneNode* cur = stack.back(); stack.pop_back();
            for (auto* c : cur->children) stack.push_back(c);
            m_byObjUUID.erase(cur->objUUID);
        }
        // detach from parent/roots
        if (n->parent) {
            auto& siblings = n->parent->children;
            siblings.erase(eastl::remove(siblings.begin(), siblings.end(), n), siblings.end());
        }
        else {
            m_roots.erase(eastl::remove(m_roots.begin(), m_roots.end(), n), m_roots.end());
        }
        // optional: actually remove storage for the subtree; otherwise mark deleted
        // (erasing from 'nodes' invalidates pointers; prefer a free-list or tombstone flag).
    }
};
