#include <Jolt/Jolt.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>

#include <Scene.h>
#include <Component/Component.h>
#include <Component/ComponentType.h>
#include <Component/TransformComponent.h>
#include <Component/RenderComponent.h>
#include <Component/PhysicsComponent.h>
#include <Component/LuaComponent.h>
#include <Component/CharacterComponent.h>
#include <Component/CharacterControllerComponent.h>

#include <Utils/StringUtils.h>

#include <GameObject/GameObject.h>
#include <GameObject/Shapes/ShapeCollection.h>
#include <GameObject/Lighting/LightCollection.h>
#include <GameObject/EditorObjectFactory.h>
#include <GameObject/GameObjectFactory.h>

#include <PlayerObject/PlayerObject.h>

#include <ParticleSystem/ParticleSystem.h>
#include <ParticleSystem/ParticleEmitterComponent.h>

#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Utils/Camera.h>
#include <Graphics/GraphicsResources/Texture.h>

#include <Serialization/GraphicsSerialization.h>


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

        j["m_uvMultiplier"] = { m_assignedComponent->m_uvMultiplier.x, m_assignedComponent->m_uvMultiplier.y };

        j["m_localPosition"] = { m_assignedComponent->m_localPosition.x, m_assignedComponent->m_localPosition.y, m_assignedComponent->m_localPosition.z };
        j["m_localRotation"] = { m_assignedComponent->m_localRotation.x, m_assignedComponent->m_localRotation.y, m_assignedComponent->m_localRotation.z };
        j["m_localScaleFactor"] = { m_assignedComponent->m_localScaleFactor.x, m_assignedComponent->m_localScaleFactor.y, m_assignedComponent->m_localScaleFactor.z };
    }
    else {
        j["m_position"] = { 0.0f, 0.0f, 0.0f };
        j["m_rotation"] = { 0.0f, 0.0f, 0.0f };
        j["m_scaleFactor"] = { 1.0f, 1.0f, 1.0f };

        j["m_uvMultiplier"] = { 1.0f, 1.0f };

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

    if (j.contains("m_uvMultiplier") && j["m_uvMultiplier"].is_array() && j["m_uvMultiplier"].size() >= 2) {
        m_uvMultiplier.x = j["m_uvMultiplier"][0].get<float>();
        m_uvMultiplier.y = j["m_uvMultiplier"][1].get<float>();
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

    return j;
}

void RenderComponent_Info::FromJson(const json& j) {

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
    AssetPath meshPath;
    if (j.contains("Mesh")) {
        meshPath.FromJson(j["Mesh"]);
    }
    else
    {
        meshPath = AssetPath(L"Box_repeat");
    }
    auto& rm = ResourceManagerFacade::Instance();
    ResourceHandle meshHandle = rm.LoadByPath(meshPath);
    if (meshHandle.guid == 0) {
        // Error
        auto ap = AssetPath(L"Box_repeat");
        meshHandle = ResourceManagerFacade::Instance().LoadByPath(ap);
    }
    SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);

    m_mesh = eastl::shared_ptr<SE_G::Mesh>(
        meshRes,
        [](SE_G::Mesh*) {}
    );
    m_mesh->m_meshPath = meshRes->m_meshPath;

    // Texture
    AssetPath texPath;
    if (j.contains("Texture"))
    {
        texPath.FromJson(j["Texture"]);
    }
    else {
        texPath = AssetPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
    }

    ResourceHandle texHandle = rm.LoadByPath(texPath);
    if (texHandle.guid == 0) {
        // Error
        auto ap = AssetPath(
            SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
            AssetPath::AssetSource::Engine);
        texHandle = ResourceManagerFacade::Instance().LoadByPath(ap);
    }
    SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

    m_texture = eastl::shared_ptr<SE_G::Bind::Texture>(
        texRes,
        [](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
    //mc_info->SetTexture(texture);
    m_texture->m_texturePath = texRes->m_texturePath;


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
        if (j.contains("m_cullMode"))
            j.at("m_cullMode").get_to(m_cullMode);

        auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
            rc->GetDevice(), tc, "GPass", uuid);
        m_gBufferTech = static_cast<SE_G::GPassTechnique*>(rc->AddTechnique(eastl::move(gBufferTech)));

        m_gBufferTech->InitByMeshData(m_meshData);
        SetCullMode(m_cullMode);
    }
}

json MeshComponent_Info::ToJson() const
{
    json j;

    // prefer assigned component's mesh if present
    if (m_assignedComponent && m_assignedComponent->m_meshData) {
        j = m_assignedComponent->m_meshData->ToJson();
        j["m_cullMode"] = m_assignedComponent->m_cullMode;
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
        {"m_collisionLayer",  std::string{m_collisionLayer.c_str()} },
        //{"m_collisionLayer",  "MOVING" },
        {"m_friction", m_friction},
        {"m_linearDamping", m_linearDamping},
        {"m_angularDamping", m_angularDamping},
        {"m_restitution", m_restitution},
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
		//m_collisionLayer = SE::CollisionLayer{ "MOVING" };
    }

    // Collider/shape data
    if (j.contains("collider") && j["collider"].is_object()) {
        if (!m_colliderData) {
            m_colliderData = eastl::make_shared<SE::ColliderData>(SE::ColliderShapeType::Box);
        }
        m_colliderData->FromJson(j["collider"]);
    }
    else
    {
        m_colliderData = eastl::make_shared<SE::ColliderData>(SE::ColliderShapeType::Box);
    }

    m_friction = j.contains("m_friction") ? j["m_friction"].get<float>() : 0.2f;
    m_linearDamping = j.contains("m_linearDamping") ? j["m_linearDamping"].get<float>() : 0.05f;
    m_angularDamping = j.contains("m_angularDamping") ? j["m_angularDamping"].get<float>() : 0.05f;
    m_restitution = j.contains("m_restitution") ? j["m_restitution"].get<float>() : 0.0f;
}

void PhysicsComponent::FromJson(const json& j) {

    PhysicsComponent_Info info; info.FromJson(j);
    
    if (info.m_collisionLayer == "NON_MOVING")  m_objectLayer = SE::Layers::NON_MOVING;
    else if (info.m_collisionLayer == "MOVING")      m_objectLayer = SE::Layers::MOVING;
    else if (info.m_collisionLayer == "TRIGGER")      m_objectLayer = SE::Layers::TRIGGER;
    else                                         m_objectLayer = SE::Layers::NON_MOVING;
    
    // m_objectLayer = SE::Layers::MOVING;

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
        m_colliderData = *info.m_colliderData;  
    }

    m_friction = j.contains("m_friction") ? j["m_friction"].get<float>() : 0.2f;
    m_linearDamping = j.contains("m_linearDamping") ? j["m_linearDamping"].get<float>() : 0.05f;
    m_angularDamping = j.contains("m_angularDamping") ? j["m_angularDamping"].get<float>() : 0.05f;
    m_restitution = j.contains("m_restitution") ? j["m_restitution"].get<float>() : 0.0f;
}

// ----------------- LuaComponent -----------------

json LuaComponent_Info::ToJson() const {
    json j;
    j = nlohmann::json{
        {"scriptPath", scriptPath.ToJson() },
        // {"selectedLuaFile", selectedLuaFile},
        {"scriptLoaded", scriptLoaded}
    };
    return j;
}

void LuaComponent_Info::FromJson(const json& j) {
    if (j.contains("scriptPath")) {
		scriptPath.FromJson(j["scriptPath"]);
        // std::string stdPath = j.at("scriptPath").get<std::string>();
        // scriptPath = eastl::string(stdPath.c_str());
    }
    // if (j.contains("selectedLuaFile") && j["selectedLuaFile"].is_number()) {
    //     selectedLuaFile = j.at("selectedLuaFile").get<int>();
    // }
    if (j.contains("scriptLoaded") && j["scriptLoaded"].is_boolean()) {
        scriptLoaded = j.at("scriptLoaded").get<bool>();
    }
}

void LuaComponent::FromJson(const json& j, GameObject* obj) {

    LuaComponent_Info info;
    info.FromJson(j);

    if (!info.scriptPath.m_assetRelativePath.empty()) {
        Init(obj, info.scriptPath);
    }
}

// ----------------- CharacterComponent -----------------

json CharacterComponent_Info::ToJson() const
{
    json j;
    j = nlohmann::json{
        "IsPlayerControlled", m_assignedComponent->IsPlayerControlled,
		// "Yaw", m_assignedComponent->Yaw,
		// "Pitch", m_assignedComponent->Pitch
    };
    return j;
}

void CharacterComponent_Info::FromJson(const json& j)
{
    if (!m_assignedComponent)
        m_assignedComponent = eastl::make_unique<CharacterComponent>();
    m_assignedComponent->FromJson(j);
}

void CharacterComponent::FromJson(const json& j)
{
    if (j.contains("IsPlayerControlled") && j["IsPlayerControlled"].is_boolean()) {
        IsPlayerControlled = j["IsPlayerControlled"].get<bool>();
    }
    // if (j.contains("Yaw") && j["Yaw"].is
}

// ----------------- CharacterControllerComponent -----------------

json CharacterControllerComponent_Info::ToJson() const
{
    json j;
    if (m_assignedComponent) {
        j = nlohmann::json{
            {"MoveSpeed", m_assignedComponent->MoveSpeed},
            {"Acceleration", m_assignedComponent->Acceleration},
            {"AirAcceleration", m_assignedComponent->AirAcceleration},
            {"JumpSpeed", m_assignedComponent->JumpSpeed},
            {"Gravity", m_assignedComponent->Gravity},
            {"MaxFallSpeed", m_assignedComponent->MaxFallSpeed},
            {"EnableStickToFloor", m_assignedComponent->EnableStickToFloor},
            {"EnableWalkStairs", m_assignedComponent->EnableWalkStairs},
            {"StepHeight", m_assignedComponent->StepHeight},
            {"MaxSlopeAngle", m_assignedComponent->MaxSlopeAngle}
        };


        if (m_assignedComponent->m_colliderData) {
            j["collider"] = m_assignedComponent->m_colliderData->ToJson();
        }
    }
    return j;
}

void CharacterControllerComponent_Info::FromJson(const json& j)
{
    if (!m_assignedComponent)
        m_assignedComponent = eastl::make_unique<CharacterControllerComponent>();
    m_assignedComponent->FromJson(j);
}

void CharacterControllerComponent::FromJson(const json& j)
{
    if (j.contains("MoveSpeed") && j["MoveSpeed"].is_number_float()) {
        MoveSpeed = j["MoveSpeed"].get<float>();
    }
    if (j.contains("Acceleration") && j["Acceleration"].is_number_float()) {
        Acceleration = j["Acceleration"].get<float>();
    }
    if (j.contains("AirAcceleration") && j["AirAcceleration"].is_number_float()) {
        AirAcceleration = j["AirAcceleration"].get<float>();
    }
    if (j.contains("JumpSpeed") && j["JumpSpeed"].is_number_float()) {
        JumpSpeed = j["JumpSpeed"].get<float>();
    }
    if (j.contains("Gravity") && j["Gravity"].is_number_float()) {
        Gravity = j["Gravity"].get<float>();
    }
    if (j.contains("MaxFallSpeed") && j["MaxFallSpeed"].is_number_float()) {
        MaxFallSpeed = j["MaxFallSpeed"].get<float>();
    }
    if (j.contains("EnableStickToFloor") && j["EnableStickToFloor"].is_number_float()) {
        EnableStickToFloor = j["EnableStickToFloor"].get<float>();
    }
    if (j.contains("EnableWalkStairs") && j["EnableWalkStairs"].is_number_float()) {
        EnableWalkStairs = j["EnableWalkStairs"].get<float>();
    }
    if (j.contains("StepHeight") && j["StepHeight"].is_number_float()) {
        StepHeight = j["StepHeight"].get<float>();
    }
    if (j.contains("MaxSlopeAngle") && j["MaxSlopeAngle"].is_number_float()) {
        MaxSlopeAngle = j["MaxSlopeAngle"].get<float>();
    }
    
    // Collider/shape data
    if (j.contains("collider") && j["collider"].is_object()) {
        if (!m_colliderData) {
            m_colliderData = eastl::make_shared<SE::ColliderData>(SE::ColliderShapeType::Capsule);
        }
        m_colliderData->FromJson(j["collider"]);
    }
    else
    {
        m_colliderData = eastl::make_shared<SE::ColliderData>(SE::ColliderShapeType::Capsule);
        m_colliderData->m_settings.data.asCapsule.m_radius = 0.35f;
        m_colliderData->m_settings.data.asCapsule.m_height = 1.8f;
    }
}

void CharacterControllerComponent::FromJson(const json& j, PhysicsSystem* physicsSystem,
    TransformComponent* transformComp, SE::UUID uuid)
{
    if (Initialized) {
        // If already initialized, we need to destroy the existing character before reinitializing
        DestroyCharacter();
    }
    FromJson(j);
    Initialize(physicsSystem, transformComp, uuid);
}

// ----------------- GameObject_Info -----------------


json GameObject_Info::ToJson() const {
    json j;
    j["m_name"] = m_name.c_str();
    j["m_UUID"] = (uint64_t)m_UUID;
	SE::UUIDhilo uuidhilo = m_UUID.GetHilo();
	j["m_UUID_hi"] = (uint32_t)uuidhilo.hi; // for debugging
    j["m_UUID_lo"] = (uint32_t)uuidhilo.lo; // for debugging
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
            case SE::ComponentType::TRIGGER:   key = "Trigger"; break;
            case SE::ComponentType::LUA:       key = "Lua"; break;
            case SE::ComponentType::MESH:                key = "Mesh"; break;
            case SE::ComponentType::PERCEPTION:          key = "Perception"; break;
            case SE::ComponentType::BEHAVIOR:            key = "Behavior"; break;
            case SE::ComponentType::PARTICLE_EMITTER:    key = "ParticleEmitter"; break;
            default: continue;
        }
        j["components"][key.c_str()] = compPtr->ToJson();
    }
    return j;
}

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
                case ShapeObjectType::Plane:
                    go = GameObjectFactory::CreatePlaneObject(renderSystem, objJ);
                    break;
                case ShapeObjectType::Sphere:
                    go = GameObjectFactory::CreateSphereObject(renderSystem, objJ);
                    break;
                case ShapeObjectType::Geosphere:
                    go = GameObjectFactory::CreateGeosphereObject(renderSystem, objJ);
                    break;
                case ShapeObjectType::Cylinder:
                    go = GameObjectFactory::CreateCylinderObject(renderSystem, objJ);
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
                go = GameObjectFactory::CreateParticleEmitter(
                    renderSystem->m_particleSystem.get(), objJ);

                break;
            }
            case GameObjectGroup::Other:
                break;
            default:
                break;
            }

            if (go) {
                if (objJ.contains("m_name"))
                {
                    go->m_name = objJ["m_name"].get<std::string>().c_str();
                }

                if (objJ["components"].contains("Mesh") &&
                    !go->HasComponent<MeshComponent>()) {
                    auto c = go->AddComponent<MeshComponent>();
                    c->FromJson(objJ["components"]["Mesh"],
                        renderSystem->GetDevice(),
                        go->GetComponent<RenderComponent>().get(),
                        go->GetComponent<TransformComponent>().get(),
                        go->m_UUID);
                }
                
                if (objJ["components"].contains("Physics")) {
                    auto c = go->AddComponent<PhysicsComponent>(
                        go->m_UUID, go->GetComponent<TransformComponent>().get());
                    c->FromJson(objJ["components"]["Physics"]);
                    //physicsSystem->CreateAndBody(c);
                }

                if (objJ["components"].contains("Trigger")) {
                    auto c = go->AddComponent<TriggerComponent>(
                        go->m_UUID, go->GetComponent<TransformComponent>().get());
                    c->FromJson(objJ["components"]["Trigger"]);
                }

                if (objJ["components"].contains("Perception")) {
                    auto c = go->AddComponent<PerceptionComponent>(go->m_UUID);
                    c->FromJson(objJ["components"]["Perception"]);
                }

                if (objJ["components"].contains("Behavior")) {
                    auto c = go->AddComponent<BehaviorController>(go->m_UUID);
                    c->FromJson(objJ["components"]["Behavior"]);
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

    GetInstance().FlushDestructionQueue();
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

eastl::unique_ptr<GameObject_Info> Scene_Info::JsonToGameObject_Info(
    eastl::shared_ptr<Scene_Info> scene,
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera, const json& objJ)
{
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
        case ShapeObjectType::Plane:
            //go = EditorObjectFactory::CreateDefaultPlaneObject(renderSystem, objJ);
            go = PlaneShapeObject_Info::FromJson(renderSystem, objJ);
            break;
        case ShapeObjectType::Sphere:
            //go = EditorObjectFactory::CreateDefaultSphereObject(renderSystem, objJ);
            go = SphereShapeObject_Info::FromJson(renderSystem, objJ);
            break;
        case ShapeObjectType::Geosphere:
            //go = EditorObjectFactory::CreateDefaultGeosphereObject(renderSystem, objJ);
            go = GeosphereShapeObject_Info::FromJson(renderSystem, objJ);
            break;
        case ShapeObjectType::Cylinder:
            //go = EditorObjectFactory::CreateDefaultCylinderObject(renderSystem, objJ);
            go = CylinderShapeObject_Info::FromJson(renderSystem, objJ);
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
        if (scene->m_playerObject == SE::UUID(0u))
        {
            go = eastl::make_unique<PlayerObject_Info>(objJ, renderSystem);
            auto playerObj = static_cast<PlayerObject_Info*>(go.get());
            playerObj->AssignSceneToCamera(scene.get());
            scene->m_playerObject = playerObj->m_UUID;
        }
        else
        {
            printSunshineErrorMessage("Only one PlayerObject can exist in scene");
        }
    }
    break;
    case GameObjectGroup::ParticleEmitter:
    {
        go = EditorObjectFactory::CreateParticleEmitter(
            renderSystem->m_particleSystem.get(), objJ);

        break;
    }
    case GameObjectGroup::Other:
        break;
    default:
        break;
    }


    if (go) {
        if (objJ.contains("m_name"))
        {
            go->m_name = objJ["m_name"].get<std::string>().c_str();
        }

        if (objJ["components"].contains("Mesh") &&
            !go->HasComponent<MeshComponent_Info>()) {
            auto c = go->AddComponent<MeshComponent_Info>();
            c->FromJson(objJ["components"]["Mesh"],
                renderSystem->GetDevice(),
                go->GetComponent<RenderComponent_Info>().get(),
                go->GetComponent<TransformComponent_Info>().get(),
                go->m_UUID);
        }

        if (objJ["components"].contains("Physics")
            && objJ["m_group"] != GameObjectGroup::Player)
        {
            auto c = go->AddComponent<PhysicsComponent_Info>(
                go->GetComponent<RenderComponent_Info>().get(),
                go->GetComponent<TransformComponent_Info>().get());
            c->FromJson(objJ["components"]["Physics"]);
            //physicsSystem->CreateAndBody(c);
        }

        if (objJ["components"].contains("Perception")) {
            auto c = go->AddComponent<PerceptionComponent_Info>();
            c->FromJson(objJ["components"]["Perception"]);
        }

        if (objJ["components"].contains("Trigger")) {
            auto c = go->AddComponent<TriggerComponent_Info>(
                go->GetComponent<RenderComponent_Info>().get(),
                go->GetComponent<TransformComponent_Info>().get());
            c->FromJson(objJ["components"]["Trigger"]);
        }

        if (objJ["components"].contains("Behavior")) {
            auto c = go->AddComponent<BehaviorController_Info>();
            c->FromJson(objJ["components"]["Behavior"]);
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

    }
    return go;
}

eastl::shared_ptr<Scene_Info> Scene_Info::FromJson(
    SE_G::DeferredRenderer* renderSystem,
    eastl::shared_ptr<SE_G::Camera> camera,
    const json& j)
{
    auto scene = eastl::make_shared<Scene_Info>();

    if (j.contains("gameObjects") && j["gameObjects"].is_array()) {
        for (const auto& objJ : j["gameObjects"]) {
            eastl::unique_ptr<GameObject_Info> go = JsonToGameObject_Info(
                scene, renderSystem, camera, objJ);
            if (go)
            {
                scene->AddGameObject(eastl::move(go));
            }
        }
    }
    scene->RestoreParents();
    return scene;
}
