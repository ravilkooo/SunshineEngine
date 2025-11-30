#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>

#include <Scene.h>
#include <Component/Component.h>
#include <Component/ComponentType.h>
#include <Component/TransformComponent.h>
#include <Component/RenderComponent.h>
#include <Component/PhysicsComponent.h>
#include <Component/LuaComponent.h>

#include <Utils/StringUtils.h>

#include <GameObject/GameObject.h>
#include <GameObject/Shapes/ShapeCollection.h>
#include <GameObject/Lighting/LightCollection.h>
#include <GameObject/EditorObjectFactory.h>
#include <GameObject/GameObjectFactory.h>

#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Utils/Camera.h>

#include <Serialization/GraphicsSerialization.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapSuleShape.h>
#include <Jolt/Physics/Collision/Shape/TaperedCapsuleShape.h>

#include <Physics/PhysicsSystem.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

// To-do: Modify all FromJson functions toi return pair [ bool, object ]
// first param for checking is deserialization was succesful

// ----------------- TransformComponent -----------------
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

void TransformComponent_Info::FromJson(const json& j, ID3D11Device* device) {
    if (!m_assignedComponent)
        m_assignedComponent = eastl::make_unique<TransformComponent>(device);
    m_assignedComponent->FromJson(j);
}

void TransformComponent::FromJson(const json& j)
{


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

// ----------------- RenderComponent -----------------
json RenderComponent_Info::ToJson() const {
    json j;
    
    /*
    //j["techniques"] = json::array();
    for (const auto& t : techniques) {
        json j_t;

        if (t == "GPass")
        {
            j_t["Mesh"] = GetCurrentMeshPath().c_str();
            j_t["Texture"] = GetCurrentTexturePath().c_str();
            j_t["Sampler"] = GetCurrentTextureSampler();

            j["techniques"]["GPass"] = j_t;
        }
    }
    */

    return j;
}
//void RenderComponent_Info::RestoreRenderTechniques(
//    GameObject_Info* gameObject,
//    const json& j)
//{
//    
//}

void RenderComponent_Info::FromJson(const json& j) {
    //techniques.clear();
    /*
    if (j.contains("techniques"))
    {
        if (j["techniques"].contains("GPass"))
        {
            eastl::string meshFullPath = j["techniques"]["GPass"]["Mesh"].get<std::string>().c_str();
            eastl::wstring textureFullPath = j["techniques"]["GPass"]["Texture"].get<std::wstring>().c_str();
            SE_G::Bind::SamplerPreset samplePreset = j["techniques"]["GPass"]["Sampler"];

        }
    }
    */
    
    /*
    if (j.contains("techniques") && j["techniques"].is_array()) {
        for (const auto& v : j["techniques"]) {
            techniques.insert( StdToEASTLString(v.get<std::string>()));
        }
    }
    */
}

// ----------------- MeshComponent -----------------

json MeshComponent_Info::ToJson() const
{
    json j;

    // prefer assigned component's mesh if present
    if (m_assignedComponent && m_assignedComponent->GetMesh()) {
        auto path = m_assignedComponent->GetMesh()->GetCurrentMeshPath();
        if (!path.empty())
            j["Mesh"] = path.c_str();
    }

    return j;
}

void MeshComponent_Info::FromJson(const json& j, ID3D11Device* device)
{
    if (j.contains("Mesh") && j["Mesh"].is_string()) {
        // remember the path so we can reconstruct later
        auto meshPath = StdToEASTLString(j["Mesh"].get<std::string>());

        // ensure an assigned component exists
        if (!m_assignedComponent)
            m_assignedComponent = eastl::make_unique<MeshComponent>();

        // If we have a valid D3D device at deserialization time, create the Mesh
        if (device) {
            auto loaded = eastl::make_shared<SE_G::Mesh>(device, meshPath);
            m_assignedComponent->SetMesh(loaded);
        }
    }
}

void MeshComponent::FromJson(const json& j, ID3D11Device* device)
{
    if (j.contains("Mesh") && j["Mesh"].is_string()) {
        eastl::string path = StdToEASTLString(j["Mesh"].get<std::string>());
        if (device && !path.empty()) {
            auto mesh = eastl::make_shared<SE_G::Mesh>(device, path);
            SetMesh(mesh);
        }
    }
}


// ----------------- PhysicsComponent -----------------
json PhysicsComponent_Info::ToJson() const {
    json j;
    j = nlohmann::json{
        {"m_motion",        m_motion},
        {"m_activation",    m_activation},
        {"m_collisionLayer",  std::string{m_collisionLayer.c_str()} }
    };

    if (m_colliderData) {
        j["collider"] = m_colliderData->ToJson();
    }
    return j;
}

void PhysicsComponent_Info::FromJson(const json& j) {
    // Read motion + activation (enums use NLOHMANN_JSON_SERIALIZE_ENUM)
    if (j.contains("m_motion")) j.at("m_motion").get_to(m_motion);
    if (j.contains("m_activation")) j.at("m_activation").get_to(m_activation);

    if (j.contains("m_collisionLayer") && j["m_collisionLayer"].is_string()) {
        m_collisionLayer = SE::CollisionLayer{ j.at("m_collisionLayer").get<std::string>().c_str() };
    }

    // Collider/shape data
    if (j.contains("collider") && j["collider"].is_object()) {
        if (!m_colliderData) {
            m_colliderData = eastl::make_shared<SE::ColliderData>(SE::ColliderShapeType::Box);
        }
        m_colliderData->FromJson(j["collider"]);
    }

}

void PhysicsComponent::FromJson(const json& j) {

    PhysicsComponent_Info info; info.FromJson(j);

    if (info.m_collisionLayer == "NON_MOVING")  m_objectLayer = SE::Layers::NON_MOVING;
    else if (info.m_collisionLayer == "MOVING")      m_objectLayer = SE::Layers::MOVING;
    else                                         m_objectLayer = SE::Layers::NON_MOVING;

    switch (info.m_motion) {
        case SE::PhysicsMotionType::Static:    m_motionType = JPH::EMotionType::Static; break;
        case SE::PhysicsMotionType::Kinematic: m_motionType = JPH::EMotionType::Kinematic; break;
        case SE::PhysicsMotionType::Dynamic:   m_motionType = JPH::EMotionType::Dynamic; break;
    }

    switch (info.m_activation) {
        case SE::PhysicsActivation::Activate:     m_activation = JPH::EActivation::Activate; break;
        case SE::PhysicsActivation::DontActivate: m_activation = JPH::EActivation::DontActivate; break;
    }

    // Deserialize shape from collider data
    if (info.m_colliderData) {
        const SE::ColliderData& colliderData = *info.m_colliderData;
        JPH::ShapeSettings::ShapeResult shapeResult;

        switch (colliderData.m_shapeType) {
            case SE::ColliderShapeType::Box: {
                JPH::BoxShapeSettings boxSettings(
                    JPH::Vec3(
                        colliderData.m_settings.data.asBox.m_size.x * 0.5f,
                        colliderData.m_settings.data.asBox.m_size.y * 0.5f,
                        colliderData.m_settings.data.asBox.m_size.z * 0.5f
                    )
                );
                shapeResult = boxSettings.Create();
                break;
            }
            case SE::ColliderShapeType::Sphere: {
                JPH::SphereShapeSettings sphereSettings(colliderData.m_settings.data.asSphere.m_radius);
                shapeResult = sphereSettings.Create();
                break;
            }
            case SE::ColliderShapeType::Capsule: {
                JPH::CapsuleShapeSettings capsuleSettings(
                    colliderData.m_settings.data.asCapsule.m_height * 0.5f,
                    colliderData.m_settings.data.asCapsule.m_radius
                );
                shapeResult = capsuleSettings.Create();
                break;
            }
            case SE::ColliderShapeType::TaperedCapsule: {
                JPH::TaperedCapsuleShapeSettings taperedCapsuleSettings(
                    colliderData.m_settings.data.asTaperedCapsule.m_height * 0.5f,
                    colliderData.m_settings.data.asTaperedCapsule.m_topRadius,
                    colliderData.m_settings.data.asTaperedCapsule.m_bottomRadius
                );
                shapeResult = taperedCapsuleSettings.Create();
                break;
            }
            default:
                // Fallback to box if shape type is not recognized
                JPH::BoxShapeSettings defaultBoxSettings(JPH::Vec3(0.5f, 0.5f, 0.5f));
                shapeResult = defaultBoxSettings.Create();
                break;
        }

        if (shapeResult.IsValid()) {
            m_shape = shapeResult.Get();
        }
    }

}

// ----------------- LuaComponent -----------------



// ----------------- GameObject_Info -----------------
json GameObject_Info::ToJson() const {
    json j;
    j["m_name"] = m_name.c_str();
    j["m_UUID"] = (uint64_t)m_UUID;
    j["m_group"] = m_group;
    switch (m_group)
    {
    case GameObjectGroup::Lighting:
        j["m_type"] = m_type.m_asLight;
        break;
    case GameObjectGroup::Shapes:
        j["m_type"] = m_type.m_asShape;
        break;
    case GameObjectGroup::CustomMesh:
        break;
    case GameObjectGroup::Other:
        break;
    default:
        break;
    }

    j["components"] = json::object();
    for (auto& [ctype, compPtr] : impl->components) {
        if (!compPtr) continue;
        // Map ComponentType enum to string key
        eastl::string key;
        switch (ctype) {
            case SE::ComponentType::TRANSFORM: key = "Transform"; break;
            case SE::ComponentType::RENDER:    key = "Render"; break;
            case SE::ComponentType::PHYSICS:   key = "Physics"; break;
            case SE::ComponentType::LUA:       key = "Lua"; break;
            default: continue;
        }
        j["components"][key.c_str()] = compPtr->ToJson();
    }
    return j;
}

/*
eastl::unique_ptr<GameObject_Info> GameObject_Info::FromJson(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    const json& j)
{
    eastl::unique_ptr<GameObject_Info> out;
    GameObjectGroup objGroup = j["m_group"];
    ObjectType objType;

    switch (objGroup)
    {
        // Lighting Group
    case GameObjectGroup::Lighting:
        objType.m_asLight = j["m_type"];

        switch (objType.m_asLight)
        {
        case LightObjectType::SkyBox:
            out = eastl::make_unique<SkyBox_Info>();
            break;
        case LightObjectType::AmbientLight:
            out = eastl::make_unique<AmbientLight_Info>();
            break;
        case LightObjectType::PointLight:
            out = eastl::make_unique<PointLight_Info>();
            break;
        case LightObjectType::DirectionalLight:
            out = eastl::make_unique<DirectionalLight_Info>();
            break;
        }
        out->m_type = objType;
        break;

        // Shapes Group
    case GameObjectGroup::Shapes:
        objType.m_asShape = j["m_type"];

        switch (objType.m_asShape)
        {
        case ShapeObjectType::Box:
            out = eastl::make_unique<GameObject_Info>();
            break;
        case ShapeObjectType::Box_repeat:
            out = eastl::make_unique<GameObject_Info>();
            break;
        case ShapeObjectType::Sphere:
            out = eastl::make_unique<GameObject_Info>();
            break;
        case ShapeObjectType::Geosphere:
            out = eastl::make_unique<GameObject_Info>();
            break;
        }
        out->m_type = objType;
        break;
        
    // case GameObjectGroup::CustomMesh:
    //     break;
    // case GameObjectGroup::Other:
    //     break;
        
    default:
        out = eastl::make_unique<GameObject_Info>();
        break;
    }
    out->m_group = objGroup;

    auto out = eastl::make_unique<GameObject_Info>();
    //if (j.contains("m_name")) out->m_name = j["m_name"].get<std::string>().c_str();
    //if (j.contains("m_UUID")) out->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
    //if (j.contains("m_group")) out->m_group = static_cast<GameObjectGroup>(j["m_group"].get<int>());
    out->m_name = j["m_name"].get<std::string>().c_str();
    out->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());

    if (j.contains("components") && j["components"].is_object()) {
        // Load components in strict order: Transform -> Render -> Physics -> Lua
        eastl::shared_ptr<TransformComponent_Info> tc_info;
        if (j["components"].contains("Transform")) {
            tc_info = out->AddComponent<TransformComponent_Info>();
            //tc_info->m_assignedComponent = eastl::make_shared<TransformComponent>(renderSystem->GetDevice());
            tc_info->FromJson(j["components"]["Transform"], renderSystem->GetDevice());
        }
        if (j["components"].contains("Render")) {
            auto rc_info = out->AddComponent<RenderComponent_Info>(out->m_UUID, renderSystem);
            rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(out->m_UUID, renderSystem);
            // Restore Render techniques

            
            
            //c->FromJson(j["components"]["Render"]);
        }
        if (j["components"].contains("Physics")) {
            auto c = out->AddComponent<PhysicsComponent_Info>();
            c->FromJson(j["components"]["Physics"]);
        }
        if (j["components"].contains("Lua")) {
            auto c = out->AddComponent<LuaComponent_Info>();
            c->FromJson(j["components"]["Lua"]);
        }
    }

    return out;
}
*/

// ----------------- Scene -----------------

eastl::shared_ptr<Scene> Scene::FromJson(
    SE_G::DeferredRenderer* renderSystem,
    PhysicsSystem* physicsSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    const json& j)
{
    auto scene = eastl::make_shared<Scene>();

    if (j.contains("gameObjects") && j["gameObjects"].is_array()) {
        for (const auto& objJ : j["gameObjects"]) {
            GameObjectGroup objGroup = objJ["m_group"];
            ObjectType objType = ObjectType(objGroup, objJ["m_type"]);
            eastl::unique_ptr<GameObject> go;

            // Unique stuff for objects groups
            // objJ;
            switch (objGroup)
            {
            case GameObjectGroup::Lighting:
                switch (objType.m_asLight)
                {
                case LightObjectType::SkyBox:
                    go = eastl::make_unique<SkyBox>(
                        renderSystem, camera, objJ);
                    break;
                case LightObjectType::AmbientLight:
                    go = eastl::make_unique<AmbientLight>(
                        renderSystem, camera, objJ);
                    break;
                case LightObjectType::PointLight:
                    go = eastl::make_unique<PointLight>(
                        renderSystem, camera, objJ);
                    break;
                case LightObjectType::DirectionalLight:
                    go = eastl::make_unique<DirectionalLight>(
                        renderSystem, camera, objJ);
                    break;
                }
                break;

            case GameObjectGroup::Shapes:

                switch (objType.m_asShape)
                {
                case ShapeObjectType::Box:
                    go = GameObjectFactory::CreateBoxObject(renderSystem, objJ);
                    break;
                case ShapeObjectType::Sphere:
                    go = GameObjectFactory::CreateSphereObject(renderSystem, objJ);
                    break;
                case ShapeObjectType::Geosphere:
                    go = GameObjectFactory::CreateGeosphereObject(renderSystem, objJ);
                    break;
                }
                break;
            case GameObjectGroup::CustomMesh:
                break;
            case GameObjectGroup::Other:
                break;
            default:
                break;
            }

            if (go) {
                // Other components (Physics, Lua)
                
                if (objJ["components"].contains("Physics")) {
                    auto c = go->AddComponent<PhysicsComponent>(
                        go->m_UUID, go->GetComponent<TransformComponent>().get());
                    c->FromJson(objJ["components"]["Physics"]);
                    physicsSystem->CreateAndAddBody(c.get());
                    //physicsSystem->CreateAndBody(c);
                }
                
                scene->AddGameObject(eastl::move(go));
            }
        }
    }
    return scene;
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

eastl::shared_ptr<Scene_Info> Scene_Info::FromJson(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    const json& j)
{
    auto scene = eastl::make_shared<Scene_Info>();
    if (j.contains("gameObjects") && j["gameObjects"].is_array()) {
        for (const auto& objJ : j["gameObjects"]) {
            GameObjectGroup objGroup = objJ["m_group"];
            ObjectType objType = ObjectType(objGroup, objJ["m_type"]);
            eastl::unique_ptr<GameObject_Info> go;
            // objJ;
            switch (objGroup)
            {
            case GameObjectGroup::Lighting:
                switch (objType.m_asLight)
                {
                case LightObjectType::SkyBox:
                    go = eastl::make_unique<SkyBox_Info>(
                        renderSystem, camera, objJ);
                    break;
                case LightObjectType::AmbientLight:
                    go = eastl::make_unique<AmbientLight_Info>(
                        renderSystem, camera, objJ);
                    break;
                case LightObjectType::PointLight:
                    go = eastl::make_unique<PointLight_Info>(
                        renderSystem, camera, objJ);
                    break;
                case LightObjectType::DirectionalLight:
                    go = eastl::make_unique<DirectionalLight_Info>(
                        renderSystem, camera, objJ);
                    break;
                }
                break;

            case GameObjectGroup::Shapes:
                switch (objType.m_asShape)
                {
                case ShapeObjectType::Box:
                    //go = EditorObjectFactory::CreateDefaultBoxObject(renderSystem, objJ);
                    go = BoxShapeObject_Info::FromJson(renderSystem, objJ);
                    break;
                case ShapeObjectType::Sphere:
                    //go = EditorObjectFactory::CreateDefaultSphereObject(renderSystem, objJ);
                    go = SphereShapeObject_Info::FromJson(renderSystem, objJ);
                    break;
                case ShapeObjectType::Geosphere:
                    //go = EditorObjectFactory::CreateDefaultGeosphereObject(renderSystem, objJ);
                    go = GeosphereShapeObject_Info::FromJson(renderSystem, objJ);
                    break;
                }
                break;

            case GameObjectGroup::CustomMesh:
                //go = eastl::make_unique<GameObject_Info>()
                break;
            case GameObjectGroup::Other:
                break;
            default:
                break;
            }


            if (go) {

                if (objJ["components"].contains("Physics")) {
                    auto c = go->AddComponent<PhysicsComponent_Info>(
                        go->GetComponent<RenderComponent_Info>().get(),
                        go->GetComponent<TransformComponent_Info>().get());
                    c->FromJson(objJ["components"]["Physics"]);
                    //physicsSystem->CreateAndBody(c);
                }

                scene->AddGameObject(eastl::move(go));
            }
        }
    }
    return scene;
}
