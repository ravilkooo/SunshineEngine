#include "Scene.h"
#include "GameObject.h"
#include "Component/Component.h"
#include "Component/ComponentType.h"
#include "Component/TransformComponent.h"
#include "Component/RenderComponent.h"
#include "Component/PhysicsComponent.h"
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>
#include <Utils/StringUtils.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ----------------- TransformComponent_Info -----------------
json TransformComponent_Info::ToJson() const {
    json j;
    // prefer assigned component values if present
    if (m_assignedComponent) {
        j["m_position"] = { m_assignedComponent->m_position.x, m_assignedComponent->m_position.y, m_assignedComponent->m_position.z };
        j["m_rotation"] = { m_assignedComponent->m_rotation.x, m_assignedComponent->m_rotation.y, m_assignedComponent->m_rotation.z };
        j["m_scaleFactor"] = { m_assignedComponent->m_scaleFactor.x, m_assignedComponent->m_scaleFactor.y, m_assignedComponent->m_scaleFactor.z };

        j["m_localPosition"] = { m_assignedComponent->m_localPosition.x, m_assignedComponent->m_localPosition.y, m_assignedComponent->m_localPosition.z };
        j["m_localRotation"] = { m_assignedComponent->m_localRotation.x, m_assignedComponent->m_localRotation.y, m_assignedComponent->m_localRotation.z };
        j["m_localScaleFactor"] = { m_assignedComponent->m_localScaleFactor.x, m_assignedComponent->m_localScaleFactor.y, m_assignedComponent->m_localScaleFactor.z };
    }
    else {
        j["m_position"] = { 0.0f, 0.0f, 0.0f };
        j["m_rotation"] = { 0.0f, 0.0f, 0.0f };
        j["m_scaleFactor"] = { 1.0f, 1.0f, 1.0f };

        j["m_localPosition"] = { 0.0f, 0.0f, 0.0f };
        j["m_localRotation"] = { 0.0f, 0.0f, 0.0f };
        j["m_localScaleFactor"] = { 1.0f, 1.0f, 1.0f };
    }
    return j;
}

void TransformComponent_Info::FromJson(const json& j) {
    auto comp = eastl::make_shared<TransformComponent>();
    comp->FromJson(j);
    m_assignedComponent = comp;
}

void TransformComponent::FromJson(const json& j) {
    if (j.contains("m_position") && j["m_position"].is_array() && j["m_position"].size() >= 3) {
        m_position.x = j["m_position"][0].get<float>();
        m_position.y = j["m_position"][1].get<float>();
        m_position.z = j["m_position"][2].get<float>();
    }
    if (j.contains("m_rotation") && j["m_rotation"].is_array() && j["m_rotation"].size() >= 3) {
        m_rotation.x = j["m_rotation"][0].get<float>();
        m_rotation.y = j["m_rotation"][1].get<float>();
        m_rotation.z = j["m_rotation"][2].get<float>();
    }
    if (j.contains("m_scaleFactor") && j["m_scaleFactor"].is_array() && j["m_scaleFactor"].size() >= 3) {
        m_scaleFactor.x = j["m_scaleFactor"][0].get<float>();
        m_scaleFactor.y = j["m_scaleFactor"][1].get<float>();
        m_scaleFactor.z = j["m_scaleFactor"][2].get<float>();
    }

    if (j.contains("m_localPosition") && j["m_localPosition"].is_array() && j["m_localPosition"].size() >= 3) {
        m_localPosition.x = j["m_localPosition"][0].get<float>();
        m_localPosition.y = j["m_localPosition"][1].get<float>();
        m_localPosition.z = j["m_localPosition"][2].get<float>();
    }
    if (j.contains("m_localRotation") && j["m_localRotation"].is_array() && j["m_localRotation"].size() >= 3) {
        m_localRotation.x = j["m_localRotation"][0].get<float>();
        m_localRotation.y = j["m_localRotation"][1].get<float>();
        m_localRotation.z = j["m_localRotation"][2].get<float>();
    }
    if (j.contains("m_localScaleFactor") && j["m_localScaleFactor"].is_array() && j["m_localScaleFactor"].size() >= 3) {
        m_localScaleFactor.x = j["m_localScaleFactor"][0].get<float>();
        m_localScaleFactor.y = j["m_localScaleFactor"][1].get<float>();
        m_localScaleFactor.z = j["m_localScaleFactor"][2].get<float>();
    }
}

// ----------------- RenderComponent_Info -----------------
json RenderComponent_Info::ToJson() const {
    json j;
    j["techniques"] = json::array();
    for (const auto& t : techniques) j["techniques"].push_back(t.c_str());
    return j;
}

void RenderComponent_Info::FromJson(const json& j) {
    techniques.clear();
    if (j.contains("techniques") && j["techniques"].is_array()) {
        for (const auto& v : j["techniques"]) {
            techniques.insert( StdToEASTLString(v.get<std::string>()));
        }
    }
}

// ----------------- PhysicsComponent_Info -----------------
json PhysicsComponent_Info::ToJson() const {
    json j;
    j["collisionLayer"] = m_collisionLayer.c_str();
    return j;
}

void PhysicsComponent_Info::FromJson(const json& j) {
    if (j.contains("collisionLayer")) m_collisionLayer = j["collisionLayer"].get<std::string>().c_str();
}

// ----------------- GameObject_Info -----------------
json GameObject_Info::ToJson() const {
    json j;
    j["m_name"] = m_name.c_str();
    j["m_UUID"] = (uint64_t)m_UUID;
    j["m_group"] = static_cast<int>(m_group);

    j["components"] = json::array();
    for (auto& [ctype, compPtr] : impl->components) {
        if (!compPtr) continue;
        json compJ;
        /*
        switch (compPtr->StaticComponentType()) {
            compJ["type"] = static_cast<int>(ctype);
        }
        */
        compJ["type"] = "type";
        compJ["data"] = compPtr->ToJson();
        j["components"].push_back(compJ);
    }
    return j;
}

eastl::unique_ptr<GameObject_Info> GameObject_Info::FromJson(const json& j) {
    auto out = eastl::make_unique<GameObject_Info>();
    if (j.contains("m_name")) out->m_name = j["m_name"].get<std::string>().c_str();
    if (j.contains("m_UUID")) out->m_UUID = Sunshine::UUID(j["m_UUID"].get<uint64_t>());
    if (j.contains("m_group")) out->m_group = static_cast<GameObjectGroup>(j["m_group"].get<int>());

    if (j.contains("components") && j["components"].is_array()) {
        for (const auto& compJ : j["components"]) {
            if (!compJ.contains("type")) continue;
            int t = compJ["type"].get<int>();
            ComponentType ctype = static_cast<ComponentType>(t);
            const json& data = compJ.contains("data") ? compJ["data"] : json::object();

            switch (ctype) {
            case ComponentType::TRANSFORM: {
                auto c = out->AddComponent<TransformComponent_Info>();
                c->FromJson(data);
                break;
            }
            case ComponentType::RENDER: {
                auto c = out->AddComponent<RenderComponent_Info>();
                c->FromJson(data);
                break;
            }
            case ComponentType::PHYSICS: {
                auto c = out->AddComponent<PhysicsComponent_Info>();
                c->FromJson(data);
                break;
            }
            default:
                // Unknown/unsupported component type - skip
                break;
            }
        }
    }

    return out;
}

// ----------------- Scene_Info -----------------
json Scene_Info::ToJson() const {
    json j;
    j["gameObjects"] = json::array();
    for (auto& uuid : gameObjects) {
        auto it = uuidToObjectMap.find(uuid);
        if (it != uuidToObjectMap.end() && it->second) {
            j["gameObjects"].push_back(it->second->ToJson());
        }
    }
    return j;
}

eastl::shared_ptr<Scene_Info> Scene_Info::FromJson(const json& j) {
    auto scene = eastl::make_shared<Scene_Info>();
    if (j.contains("gameObjects") && j["gameObjects"].is_array()) {
        for (const auto& objJ : j["gameObjects"]) {
            auto go = GameObject_Info::FromJson(objJ);
            if (go) {
                scene->AddGameObject(eastl::move(go));
            }
        }
    }
    return scene;
}
