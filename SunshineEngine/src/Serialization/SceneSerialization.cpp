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

#include <PlayerObject/PlayerObject.h>

#include <ParticleSystem/ParticleSystem.h>
#include <ParticleSystem/ParticleEmitter.h>

#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Utils/Camera.h>
#include <Graphics/GraphicsResources/Texture.h>

#include <Serialization/GraphicsSerialization.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapSuleShape.h>
#include <Jolt/Physics/Collision/Shape/TaperedCapsuleShape.h>

#include <Physics/PhysicsSystem.h>

#include <ResourceManager/ResourceManagerFacade.h>

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

json MeshData::ToJson() const
{
    json j;
    if (m_mesh) {
        try { j["Mesh"] = m_mesh->GetCurrentMeshPath().ToJson(); }
        catch (...) {}
    }
    if (m_texture) {
        try { j["Texture"] = m_texture->m_texturePath.ToJson(); }
        catch (...) {}
    }
    if (m_textureSampler) {
        try { j["Sampler"] = static_cast<int>(m_textureSampler->GetPreset()); }
        catch (...) {}
    }
    return j;
}

void MeshData::FromJson(const json& j, ID3D11Device* device)
{
    // Mesh
    if (j.contains("Mesh")) {

        AssetPath meshPath;
        meshPath.FromJson(j["Mesh"]);

        m_mesh = eastl::make_shared<SE_G::Mesh>(device, meshPath);
    }
    else
    {
        m_mesh = eastl::make_shared<SE_G::Mesh>(device, AssetPath(L"Box_repeat"));
    }

    // Texture
    AssetPath texPath;
    if (j.contains("Texture"))
    {
        texPath.FromJson(j["Texture"]);
    }
    else {
        texPath = AssetPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
    }
    /*
    m_texture = eastl::make_shared<SE_G::Bind::Texture>(
        device,
        texPath, 0u,
        SE_G::Bind::PipelineStage::PIXEL_SHADER);
    */
    auto& rm = ResourceManagerFacade::Instance();
    ResourceHandle texHandle = rm.LoadByPath(texPath);
    SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

    if (texRes)
    {
        m_texture = eastl::shared_ptr<SE_G::Bind::Texture>(
            texRes,
            [](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
        //mc_info->SetTexture(texture);
    }
    else
    {
        m_texture = eastl::make_shared<SE_G::Bind::Texture>(
            device,
            AssetPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine),
            0u,
            SE_G::Bind::PipelineStage::PIXEL_SHADER);
        //mc_info->SetTexture(texture);
    }



    // Sampler
    if (j.contains("Sampler")) {
        int preset = j["Sampler"].get<int>();
        m_textureSampler = eastl::make_shared<SE_G::Bind::Sampler>(device, static_cast<SE_G::Bind::SamplerPreset>(preset));
    }
    else {
        m_textureSampler = eastl::make_shared<SE_G::Bind::Sampler>(device, SE_G::Bind::SamplerPreset::Wrap);
    }
}

void MeshComponent::FromJson(const json& j, ID3D11Device* device,
    RenderComponent* rc, TransformComponent* tc,
    SE::UUID uuid)
{
    if (j.contains("Mesh")) {
        m_meshData = eastl::make_shared<MeshData>();
        m_meshData->FromJson(j, device);

        auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
            rc->GetDevice(), tc, "GPass", uuid);
        m_gBufferTech = static_cast<SE_G::GPassTechnique*>(rc->AddTechnique(eastl::move(gBufferTech)));

        m_gBufferTech->InitByMeshData(m_meshData);
    }
}

json MeshComponent_Info::ToJson() const
{
    json j;

    // prefer assigned component's mesh if present
    if (m_assignedComponent && m_assignedComponent->m_meshData) {
        return m_assignedComponent->m_meshData->ToJson();
    }

    return j;
}

void MeshComponent_Info::FromJson(const json& j, ID3D11Device* device,
    RenderComponent_Info* rc_info, TransformComponent_Info* tc_info,
    SE::UUID uuid)
{
    m_assignedComponent = eastl::make_unique<MeshComponent>();
    // If there's an assigned runtime component, populate it
    if (m_assignedComponent) {
        m_assignedComponent->FromJson(j, device, rc_info->m_assignedComponent.get(),
            tc_info->m_assignedComponent.get(), uuid);


        rc_info->AddTechnique_Info(rc_info->m_assignedComponent->GetTechnique("GPass"));

        m_rc_info = rc_info;

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

        m_transformsData.m_offset = info.m_colliderData->GetTransformData().m_offset;
        m_transformsData.m_rotation = info.m_colliderData->GetTransformData().m_rotation;

        if (shapeResult.IsValid()) {
            m_shape = shapeResult.Get();
        }
    }

}

// ----------------- LuaComponent -----------------

json LuaComponent_Info::ToJson() const {
    json j;
    j = nlohmann::json{
        {"scriptPath", std::string{scriptPath.c_str()}},
        {"selectedLuaFile", selectedLuaFile},
        {"scriptLoaded", scriptLoaded}
    };
    return j;
}

void LuaComponent_Info::FromJson(const json& j) {
    if (j.contains("scriptPath") && j["scriptPath"].is_string()) {
        std::string stdPath = j.at("scriptPath").get<std::string>();
        scriptPath = eastl::string(stdPath.c_str());
    }
    if (j.contains("selectedLuaFile") && j["selectedLuaFile"].is_number()) {
        selectedLuaFile = j.at("selectedLuaFile").get<int>();
    }
    if (j.contains("scriptLoaded") && j["scriptLoaded"].is_boolean()) {
        scriptLoaded = j.at("scriptLoaded").get<bool>();
    }
}

void LuaComponent::FromJson(const json& j, GameObject* obj) {

    LuaComponent_Info info;
    info.FromJson(j);

    if (!info.scriptPath.empty()) {
        Init(obj, info.scriptPath);
    }
}

// ----------------- GameObject_Info -----------------


json GameObject_Info::ToJson() const {
    json j;
    j["m_name"] = m_name.c_str();
    j["m_UUID"] = (uint64_t)m_UUID;
    j["m_group"] = m_group;
    j["m_parent"] = m_parent.ToJson();
    

    switch (m_group)
    {
    case GameObjectGroup::Lighting:
    {
        j["m_type"] = m_type.m_asLight;
        break;
    }
    case GameObjectGroup::Shapes:
    {
        j["m_type"] = m_type.m_asShape;
        break;
    }
    case GameObjectGroup::CustomMesh:
    {
        break;
    }
    case GameObjectGroup::Player:
    {
        break;
    }
    case GameObjectGroup::Other:
    {
        break;
    }
    default:
    {
        break;
    }
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
            case SE::ComponentType::MESH:       key = "Mesh"; break;
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

void Scene::FromJson(
    SE_G::DeferredRenderer* renderSystem,
    PhysicsSystem* physicsSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    const json& j)
{

    if (j.contains("gameObjects") && j["gameObjects"].is_array()) {
        for (const auto& objJ : j["gameObjects"]) {
            GameObjectGroup objGroup = objJ["m_group"];
            eastl::unique_ptr<GameObject> go;
            ObjectType objType;
            // Unique stuff for objects groups
            // objJ;
            switch (objGroup)
            {

            case GameObjectGroup::Lighting:
                objType = ObjectType(objGroup, objJ["m_type"]);
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
                case LightObjectType::SpotLight:
                    go = eastl::make_unique<SpotLight>(
                        renderSystem, camera, objJ);
                    break;
                case LightObjectType::DirectionalLight:
                    go = eastl::make_unique<DirectionalLight>(
                        renderSystem, camera, objJ);
                    break;
                }
                break;

            case GameObjectGroup::Shapes:
                objType = ObjectType(objGroup, objJ["m_type"]);
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
            {
                go = GameObjectFactory::CreateCustomMesh(
                    renderSystem, objJ);
            }
                break;
            case GameObjectGroup::Player:
            {
                go = eastl::make_unique<PlayerObject>(objJ, renderSystem, camera);
                auto playerObj = static_cast<PlayerObject*>(go.get());
                playerObj->AssignSceneToCamera(&GetInstance());
                GetInstance().m_playerObjectUUID = playerObj->m_UUID;
                break;
            }
            case GameObjectGroup::ParticleEmitter:
            {
                go = SE::ParticleEmitter::FromJson(objJ, renderSystem->m_particleSystem.get());

                break;
            }
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

                if (objJ["components"].contains("Lua")) {
                    auto luaComp = go->AddComponent<LuaComponent>();
                    luaComp->FromJson(objJ["components"]["Lua"], go.get());
                }
                
                // Parentnes
                if (objJ.contains("m_parent"))
                {
                    go->SetParent(ParentNode<GameObject>::FromJson(objJ["m_parent"]));
                }

                auto objUUID = GetInstance().AddGameObject(eastl::move(go));
                if (objGroup == GameObjectGroup::Player)
                {
                    GetInstance().m_playerObject = static_cast<PlayerObject*>(
                        GetInstance().GetGameObjectByUUID(objUUID)
                        );
                }
            }
        }
    }
    GetInstance().RestoreParents();
}

// ----------------- Scene_Info -----------------
json Scene_Info::ToJson() const {
    json j;
    j["gameObjects"] = json::array();
    for (auto& uuid : gameObjects)
    {
        auto it = uuidToObjectMap.find(uuid);
        if (it != uuidToObjectMap.end() && it->second)
        {
            // if (it->second->m_group == GameObjectGroup::ParticleEmitter)
            //     continue;

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
            ObjectType objType;
            eastl::unique_ptr<GameObject_Info> go;
            // objJ;
            switch (objGroup)
            {
            case GameObjectGroup::Lighting:
                objType = ObjectType(objGroup, objJ["m_type"]);

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
                case LightObjectType::SpotLight:
                    go = eastl::make_unique<SpotLight_Info>(
                        renderSystem, camera, objJ);
                    break;
                }
                break;

            case GameObjectGroup::Shapes:
                objType = ObjectType(objGroup, objJ["m_type"]);

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
            {
                go = EditorObjectFactory::CreateCustomMesh(renderSystem, objJ);
            }
                break;

            case GameObjectGroup::Player:
            {
                go = eastl::make_unique<PlayerObject_Info>(objJ, renderSystem);
                auto playerObj = static_cast<PlayerObject_Info*>(go.get());
                playerObj->AssignSceneToCamera(scene.get());
                scene->m_playerObject = playerObj->m_UUID;
                break;
            }
            case GameObjectGroup::ParticleEmitter:
            {
                go = SE::ParticleEmitter_Info::FromJson(objJ, renderSystem->m_particleSystem.get());
                    
                break;
            }
            case GameObjectGroup::Other:
                break;
            default:
                break;
            }


            if (go) {

                if (objJ["components"].contains("Physics")
                    && objJ["m_group"] != GameObjectGroup::Player)
                {
                    auto c = go->AddComponent<PhysicsComponent_Info>(
                        go->GetComponent<RenderComponent_Info>().get(),
                        go->GetComponent<TransformComponent_Info>().get());
                    c->FromJson(objJ["components"]["Physics"]);
                    //physicsSystem->CreateAndBody(c);
                }

                if (objJ["components"].contains("Lua")) {
                    auto c = go->AddComponent<LuaComponent_Info>();
                    c->FromJson(objJ["components"]["Lua"]);
                }

                // Parentnes
                if (objJ.contains("m_parent"))
                {
                    go->SetParent(ParentNode<GameObject_Info>::FromJson(objJ["m_parent"]));
                }

                scene->AddGameObject(eastl::move(go));
            }
        }
    }
    scene->RestoreParents();
    return scene;
}
