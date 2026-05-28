#include <Component/CameraComponent.h>

#include <Graphics/Utils/Camera.h>

#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>

CameraComponent::CameraComponent(ID3D11Device* device, TransformComponent* trComp, SE::UUID uuid)
{
    m_camera = eastl::make_shared<SE_G::Camera>(device);
    m_camera->AssignTransformComponent(trComp);
    m_camera->SetFollowUUID(uuid);
}

CameraComponent::CameraComponent(eastl::shared_ptr<SE_G::Camera> camera, TransformComponent* trComp, SE::UUID uuid)
	: m_camera(camera)
{
    m_camera->AssignTransformComponent(trComp);
    m_camera->SetFollowUUID(uuid);
}

SE_G::Camera* CameraComponent::GetCamera()
{
	return m_camera.get();
}

void CameraComponent::FromJson(const json& j)
{
    if (!m_camera)
        return;

	if (j.contains("m_springArmParams"))
	{
		if (j["m_springArmParams"].contains("length")) {
			m_camera->m_springArmParams.length = j["m_springArmParams"]["length"].get<float>();
		}
		if (j["m_springArmParams"].contains("pitchYawRoll") && j["m_springArmParams"]["pitchYawRoll"].is_array() && j["m_springArmParams"]["pitchYawRoll"].size() >= 3) {
			m_camera->m_springArmParams.pitchYawRoll.x = j["m_springArmParams"]["pitchYawRoll"][0].get<float>();
			m_camera->m_springArmParams.pitchYawRoll.y = j["m_springArmParams"]["pitchYawRoll"][1].get<float>();
			m_camera->m_springArmParams.pitchYawRoll.z = j["m_springArmParams"]["pitchYawRoll"][2].get<float>();
		}
		if (j["m_springArmParams"].contains("rootOffset") && j["m_springArmParams"]["rootOffset"].is_array() && j["m_springArmParams"]["rootOffset"].size() >= 3) {
			m_camera->m_springArmParams.rootOffset.x = j["m_springArmParams"]["rootOffset"][0].get<float>();
			m_camera->m_springArmParams.rootOffset.y = j["m_springArmParams"]["rootOffset"][1].get<float>();
			m_camera->m_springArmParams.rootOffset.z = j["m_springArmParams"]["rootOffset"][2].get<float>();
		}
	}
	if (j.contains("cameraPitchYawRoll") && j["cameraPitchYawRoll"].is_array() && j["cameraPitchYawRoll"].size() >= 3) {
		m_camera->cameraPitchYawRoll.x = j["cameraPitchYawRoll"][0].get<float>();
		m_camera->cameraPitchYawRoll.y = j["cameraPitchYawRoll"][1].get<float>();
		m_camera->cameraPitchYawRoll.z = j["cameraPitchYawRoll"][2].get<float>();
	}
}

void CameraComponent::FromJson(const json& j, ID3D11Device* device, TransformComponent* trComp, SE::UUID uuid)
{
    m_camera = eastl::make_shared<SE_G::Camera>(device);
    m_camera->AssignTransformComponent(trComp);
    m_camera->SetFollowUUID(uuid);
    FromJson(j);
}

CameraComponent_Info::CameraComponent_Info(ID3D11Device* device, TransformComponent* trComp, SE::UUID uuid)
{
    m_assignedComponent = eastl::make_unique<CameraComponent>(device, trComp, uuid);
}

CameraComponent_Info::CameraComponent_Info(eastl::shared_ptr<SE_G::Camera> camera, TransformComponent* trComp, SE::UUID uuid)
{
    m_assignedComponent = eastl::make_unique<CameraComponent>(camera, trComp, uuid);
}

SE_G::Camera* CameraComponent_Info::GetCamera()
{
    return m_assignedComponent->m_camera.get();
}

json CameraComponent_Info::ToJson() const
{
	json j = json::object();
	j["cameraPitchYawRoll"] = {
		m_assignedComponent->m_camera->cameraPitchYawRoll.x,
		m_assignedComponent->m_camera->cameraPitchYawRoll.y,
		m_assignedComponent->m_camera->cameraPitchYawRoll.z
	};
	j["m_springArmParams"] = json::object();
	j["m_springArmParams"]["length"] = m_assignedComponent->m_camera->m_springArmParams.length;
	j["m_springArmParams"]["pitchYawRoll"] = {
		m_assignedComponent->m_camera->m_springArmParams.pitchYawRoll.x,
		m_assignedComponent->m_camera->m_springArmParams.pitchYawRoll.y,
		m_assignedComponent->m_camera->m_springArmParams.pitchYawRoll.z
	};
	j["m_springArmParams"]["rootOffset"] = {
		m_assignedComponent->m_camera->m_springArmParams.rootOffset.x,
		m_assignedComponent->m_camera->m_springArmParams.rootOffset.y,
		m_assignedComponent->m_camera->m_springArmParams.rootOffset.z
	};

    return j;
}

void CameraComponent_Info::FromJson(const json& j)
{
	if (!m_assignedComponent)
		return;
	m_assignedComponent->FromJson(j);
}

void CameraComponent_Info::FromJson(const json& j, ID3D11Device* device, TransformComponent* trComp, SE::UUID uuid)
{
	m_assignedComponent = eastl::make_unique<CameraComponent>(device, trComp, uuid);
	m_assignedComponent->FromJson(j);
}

#define CC_ADD_METHOD(k, fn) k, fn

LUA_REGISTER_COMPONENT(
    CameraComponent,
    "CameraComponent",
    /* no fields */,
    CAMERACOMPONENT_LUA_METHODS_APPLY(CC_ADD_METHOD),
    "getCameraComponent")
#undef CC_ADD_METHOD
