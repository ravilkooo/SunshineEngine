#include "GameObject/GameObject.h"

#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>
#include <Component/PhysicsComponent.h>
#include <Component/TriggerComponent.h>
#include <Component/MeshComponent.h>
#include <Component/LuaComponent.h>
#include <Component/CharacterComponent.h>
#include <Component/CameraComponent.h>
#include <Component/CharacterControllerComponent.h>

#include "AI/Perception/PerceptionComponent.h"
#include "AI/Behavior/BehaviorController.h"

#include <Utils/MathUtils.h>

#include <Graphics/GraphicsResources/Mesh.h>
#include <ResourceManager/ResourceManagerFacade.h>
//#include <Utils/StringUtils.h>

GameObject::GameObject() : impl(eastl::make_unique<GameObjectImpl>())
{
	m_UUID = SE::UUID();
};

GameObject::GameObject(SE::UUID uuid) : impl(eastl::make_unique<GameObjectImpl>())
{
	m_UUID = uuid;
};

GameObject::~GameObject()
{
	m_name.clear();
	impl->components.clear();
};

void GameObject::SetParent(ParentNode<GameObject> parent)
{
    GameObject* currNode = parent.ptr;
    while (currNode)
    {
        if (currNode == this)
        {
            printf("Cyclce parentness dependence prevented!\n");
            if (parent.ptr->m_UUID == m_parent.ptr->m_UUID)
            {
                m_parent.uuid = SE::UUID(0u);
                m_parent.ptr = nullptr;
                m_parent.attached = false;
            }
            return;
        }
        currNode = currNode->m_parent.ptr;
    }
    m_parent = parent;
    if (m_parent.uuid != SE::UUID(0u) && m_parent.ptr)
    {
        m_parent.ptr->m_children.push_back(this->m_UUID);
    }

    if (m_parent.ptr && m_parent.attached)
    {
        if (HasComponent<TransformComponent>() && m_parent.ptr->HasComponent<TransformComponent>())
        {
            GetComponent<TransformComponent>()->SetParentTransform(
                m_parent.ptr->GetComponent<TransformComponent>().get()
            );
        }
        if (HasComponent<PhysicsComponent>())
        {
            // RemoveComponent<PhysicsComponent>();
            // To-do: remove phys from every child

            // OR

            // To-do: make extended attached mode
            GetComponent<PhysicsComponent>()->SetMotionType(JPH::EMotionType::Kinematic);
        }
    }
}

void GameObject::AttachToParent(bool alreadyLocalTransform)
{
    if (!HasComponent<TransformComponent>() || !m_parent.ptr->HasComponent<TransformComponent>())
        return;

    if (!alreadyLocalTransform)
    {
        auto tc = GetComponent<TransformComponent>();
        auto tc_parent = m_parent.ptr->GetComponent<TransformComponent>();

        DXSM::Matrix newTransform = tc->GetWorldMatrix_noLocal() * tc_parent->GetWorldMatrix_noLocal().Invert();

        DXSM::Vector3 scale;
        DXSM::Vector3 rotate;
        DXSM::Vector3 translation;
        DecomposeTransform(newTransform, scale, rotate, translation);
        tc->m_scaleFactor = scale;
        tc->m_position = translation;
        tc->m_rotation = rotate;
    }


    GetComponent<TransformComponent>()->SetParentTransform(
        m_parent.ptr->GetComponent<TransformComponent>().get()
    );

    if (HasComponent<PhysicsComponent>())
    {
        // RemoveComponent<PhysicsComponent>();

        // To-do: remove phys from every child

        // OR
        // To-do: make extended attached mode
        GetComponent<PhysicsComponent>()->SetMotionType(JPH::EMotionType::Kinematic);
    }

    m_parent.attached = true;

    // To-do rewrite code to safe local transform
    // tc->m_localScaleFactor = DXSM::Vector3::One;
    // tc->m_localRotation = DXSM::Vector3::Zero;
    // tc->m_localPosition = DXSM::Vector3::Zero;
}

void GameObject::DetachFromParent()
{
    auto tc = GetComponent<TransformComponent>();
    auto tc_parent = m_parent.ptr->GetComponent<TransformComponent>();

    DXSM::Matrix newTransform = tc->GetWorldMatrix_noLocal();

    DXSM::Vector3 scale;
    DXSM::Vector3 rotate;
    DXSM::Vector3 translation;
    DecomposeTransform(newTransform, scale, rotate, translation);
    tc->m_scaleFactor = scale;
    tc->m_position = translation;
    tc->m_rotation = rotate;

    GetComponent<TransformComponent>()->SetParentTransform(
        nullptr
    );

    if (HasComponent<PhysicsComponent>())
    {
        //RemoveComponent<PhysicsComponent>();

        // To-do: make extended attached mode
        //GetComponent<PhysicsComponent>()->SetMotionType(JPH::EMotionType::Dynamic);
    }

    m_parent.attached = false;
}

GameObject_Info::GameObject_Info() : impl(eastl::make_unique<GameObject_InfoImpl>())
{
	m_UUID = SE::UUID();
};

GameObject_Info::GameObject_Info(SE::UUID uuid) : impl(eastl::make_unique<GameObject_InfoImpl>())
{
	m_UUID = uuid;
};

GameObject_Info::~GameObject_Info()
{
	m_name.clear();
	impl->components.clear();
};

void GameObject_Info::AddDefaultComponent(SE::ComponentType compType)
{
    if (impl->components.contains(compType))
    {
        return;
    }
    else
    {
        switch (compType)
        {
        case SE::ComponentType::TRANSFORM:
        case SE::ComponentType::RENDER:
            break;

        case SE::ComponentType::LUA:
        {
            // To-do:
            // Add  #include <Component/LuaComponent.h>
            // Add LuaComponent with default values

            auto lc_info = AddComponent<LuaComponent_Info>();
            lc_info->InitLuaFile();
        }
        break;

        case SE::ComponentType::PHYSICS:

            // Add PhysicsComponent with default values
        {
            if (HasComponent<CharacterControllerComponent_Info>())
            {
                printSunshineErrorMessage("Cannot add PhysicsComponent when CharacterControllerComponent exists. Please remove CharacterControllerComponent first.");
                break;
            }

            auto tc_info = GetComponent<TransformComponent_Info>();
            auto rc_info = GetComponent<RenderComponent_Info>();

            auto pc_info = AddComponent<PhysicsComponent_Info>(rc_info.get(), tc_info.get());

        }
        break;

        case SE::ComponentType::TRIGGER:

            // Add PhysicsComponent with default values
        {
            auto tc_info = GetComponent<TransformComponent_Info>();
            auto rc_info = GetComponent<RenderComponent_Info>();

            auto trigc_info = AddComponent<TriggerComponent_Info>(rc_info.get(), tc_info.get());

        }
        break;

        case SE::ComponentType::PERCEPTION:
        {
            auto percc_info = AddComponent<PerceptionComponent_Info>();
        }

        break;

        case SE::ComponentType::BEHAVIOR:
        {
            auto bc_info = AddComponent<BehaviorController_Info>();
        }

        break;

        case SE::ComponentType::MESH:

            // To-do:
            // Add  #include <Component/MeshComponent.h>
            // Add MeshComponent with default values

        {
            auto tc_info = GetComponent<TransformComponent_Info>();
            auto rc_info = GetComponent<RenderComponent_Info>();

            eastl::shared_ptr<SE_G::Mesh> meshPtr;
            AssetPath meshPath = AssetPath(L"Box_repeat");
            auto& rm = ResourceManagerFacade::Instance();
            ResourceHandle meshHandle = rm.LoadByPath(meshPath);
            SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
            meshPtr = eastl::shared_ptr<SE_G::Mesh>(
                meshRes,
                [](SE_G::Mesh*) {}
            );
            meshPtr->m_meshPath = meshRes->m_meshPath;

            auto meshComp = AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), m_UUID, meshPtr);
        }

        break;

        case SE::ComponentType::CHARACTER:

            // Add CharacterComponent with default values
        {
            auto ch_info = AddComponent<CharacterComponent_Info>();
        }
        break;


        case SE::ComponentType::CHARACTER_CONTROLLER:

            // Add PhysicsComponent with default values
        {
            if (HasComponent<PhysicsComponent_Info>())
            {
                printSunshineErrorMessage("Cannot add CharacterControllerComponent when PhysicsComponent exists. Please remove PhysicsComponent first.");
                break;
            }

            if (!HasComponent<CharacterComponent_Info>())
            {
                printSunshineErrorMessage("CharacterControllerComponent requires CharacterComponent. Please add CharacterComponent first.");
                break;
            }

            auto tc_info = GetComponent<TransformComponent_Info>();
            auto rc_info = GetComponent<RenderComponent_Info>();

            auto cc_info = AddComponent<CharacterControllerComponent_Info>(rc_info.get(), tc_info.get());

        }
        break;

        case SE::ComponentType::CAMERA:
        {
            auto rc_info = GetComponent<RenderComponent_Info>();
            auto cam_info = AddComponent<CameraComponent_Info>(rc_info->GetDevice());
        }

        break;

        default:
            break;
        }
    }
}

void GameObject_Info::SetParent(ParentNode<GameObject_Info> parent)
{
    GameObject_Info* currNode = parent.ptr;
    while (currNode)
    {
        if (currNode == this)
        {
            printf("Cyclce parentness dependence prevented!\n");
            if (parent.ptr->m_UUID == m_parent.ptr->m_UUID)
            {
                m_parent.uuid = SE::UUID(0u);
                m_parent.ptr = nullptr;
                m_parent.attached = false;
            }
            return;
        }
        currNode = currNode->m_parent.ptr;
    }
    m_parent = parent;

    if (m_parent.ptr && m_parent.attached)
    {
        if (HasComponent<TransformComponent_Info>() && m_parent.ptr->HasComponent<TransformComponent_Info>())
        {
            GetComponent<TransformComponent_Info>()->SetParentTransform(
                m_parent.ptr->GetComponent<TransformComponent_Info>().get()
            );
        }

        if (HasComponent<PhysicsComponent_Info>())
        {
            // RemoveComponent< PhysicsComponent_Info>();

            // To-do: remove phys from every child

            // OR
            // To-do: make extended attached mode
            GetComponent<PhysicsComponent_Info>()->SetMotion(SE::PhysicsMotionType::Kinematic);
        }
    }
}

void GameObject_Info::AttachToParent(bool alreadyLocalTransform)
{
    if (!HasComponent<TransformComponent_Info>() || !m_parent.ptr || !m_parent.ptr->HasComponent<TransformComponent_Info>())
        return;

    if (!alreadyLocalTransform)
    {
        auto tc = GetComponent<TransformComponent_Info>()->m_assignedComponent.get();
        auto tc_parent = m_parent.ptr->GetComponent<TransformComponent_Info>()->m_assignedComponent.get();

        DXSM::Matrix newTransform = tc->GetWorldMatrix_noLocal() * tc_parent->GetWorldMatrix_noLocal().Invert();

        DXSM::Vector3 scale;
        DXSM::Vector3 rotate;
        DXSM::Vector3 translation;
        DecomposeTransform(newTransform, scale, rotate, translation);
        tc->m_scaleFactor = scale;
        tc->m_position = translation;
        tc->m_rotation = rotate;
    }

    GetComponent<TransformComponent_Info>()->SetParentTransform(
        m_parent.ptr->GetComponent<TransformComponent_Info>().get()
    );

    if (HasComponent<PhysicsComponent_Info>())
    {
        // RemoveComponent<PhysicsComponent_Info>();
        // To-do
        // Remove PhysComp from every GrandChild
        // OR
        // To-do: make extended attached mode
        GetComponent<PhysicsComponent_Info>()->SetMotion(SE::PhysicsMotionType::Kinematic);
    }

    m_parent.attached = true;
}

void GameObject_Info::DetachFromParent()
{
    auto tc = GetComponent<TransformComponent_Info>()->m_assignedComponent.get();
    auto tc_parent = m_parent.ptr->GetComponent<TransformComponent_Info>()->m_assignedComponent.get();

    DXSM::Matrix newTransform = tc->GetWorldMatrix_noLocal();

    DXSM::Vector3 scale;
    DXSM::Vector3 rotate;
    DXSM::Vector3 translation;
    DecomposeTransform(newTransform, scale, rotate, translation);
    tc->m_scaleFactor = scale;
    tc->m_position = translation;
    tc->m_rotation = rotate;

    GetComponent<TransformComponent_Info>()->SetParentTransform(
        nullptr
    );

    if (HasComponent<PhysicsComponent_Info>())
    {
        //GetComponent<PhysicsComponent_Info>()->SetMotion(SE::PhysicsMotionType::Dynamic);
    }

    m_parent.attached = false;
}
