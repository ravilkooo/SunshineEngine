#include <GameObject/Shapes/SphereShapeObject.h>
#include <Graphics/Renderer/Technique/ColliderTechnique.h>
#include <Component/PhysicsComponent.h>
#include <Component/MeshComponent.h>
#include <Graphics/GraphicsResources/Texture.h>

SphereShapeObject_Info::SphereShapeObject_Info(SE::UUID uuid,
	SE_G::DeferredRenderer* renderSystem, SphereShapeData initData)
{
	auto device = renderSystem->GetDevice();
	m_UUID = uuid;
	m_group = GameObjectGroup::Shapes;
	m_type.m_asShape = ShapeObjectType::Sphere;
	m_name = "Sphere";
	m_shapeData = eastl::make_shared<SphereShapeData>(initData);

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>(device);

	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

	auto mesh = SE_G::Mesh::CreateSphereMesh(device, initData.Size, initData.SliceCount, initData.StackCount);
	auto mesh_info = AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), m_UUID, mesh);

	auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		rc_info->GetDevice(),
		MakeEngineAssetPath_Wstring(L"DefaultSphereTexture.dds"), 0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER);
	mesh_info->SetTexture(texture);
}

SphereShapeObject_Info::SphereShapeObject_Info(SE_G::DeferredRenderer* renderSystem, SphereShapeData initData) :
	SphereShapeObject_Info(SE::UUID(), renderSystem, initData)
{
}

eastl::unique_ptr<SphereShapeObject_Info> SphereShapeObject_Info::FromJson(
	SE_G::DeferredRenderer* renderSystem, const json& j)
{
	eastl::unique_ptr<SphereShapeObject_Info> obj = eastl::make_unique<SphereShapeObject_Info>();

	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	obj->m_shapeData = eastl::make_shared<SphereShapeData>(j["m_shapeData"].get<SphereShapeData>());
	obj->m_name = "Sphere";
	obj->m_group = GameObjectGroup::Shapes;
	obj->m_type.m_asShape = ShapeObjectType::Sphere;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc_info = obj->AddComponent<TransformComponent_Info>(device);
	if (j["components"].contains("Transform")) {
		tc_info->FromJson(j["components"]["Transform"], device);
	}

	// RenderComponent and technique
	auto rc_info = obj->AddComponent<RenderComponent_Info>(obj->m_UUID, renderSystem);

	auto newMesh = SE_G::Mesh::CreateSphereMesh(device, obj->m_shapeData->Size, obj->m_shapeData->SliceCount, obj->m_shapeData->StackCount);
	auto mesh_info = obj->AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), obj->m_UUID, newMesh);

	auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		rc_info->GetDevice(),
		MakeEngineAssetPath_Wstring(L"DefaultSphereTexture.dds"), 0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER);
	mesh_info->SetTexture(texture);

	// PhysicsComponent
	/*
	if (j["components"].contains("Physics")) {
		auto pc_info = obj->AddComponent<PhysicsComponent_Info>(rc_info.get(), tc_info.get());
		pc_info->FromJson(j["components"]["Physics"]);
	}
	else {
		
	}
	*/

	return obj;
}

DXSM::Vector3 SphereShapeObject_Info::GetSize() {
	return m_shapeData->Size;
}

uint32_t SphereShapeObject_Info::GetSliceCount() {
	return m_shapeData->SliceCount;
}

uint32_t SphereShapeObject_Info::GetStackCount() {
	return m_shapeData->StackCount;
}

void SphereShapeObject_Info::SetSize(SE_G::DeferredRenderer* renderSystem, DXSM::Vector3 newSize) {
	if (DX::XMVector3Greater(newSize, DXSM::Vector3::Zero)) {
		m_shapeData->Size = newSize;

		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateSphereMesh(renderSystem->GetDevice(),
				newSize, m_shapeData->SliceCount, m_shapeData->StackCount);
		auto mc = GetComponent<MeshComponent_Info>();
		mc->m_assignedComponent->SetMesh(newMesh);
	}
}

void SphereShapeObject_Info::SetSliceCount(SE_G::DeferredRenderer* renderSystem, uint32_t newSliceCount) {
	if (newSliceCount > 0) {
		m_shapeData->SliceCount = newSliceCount;
		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateSphereMesh(renderSystem->GetDevice(),
				m_shapeData->Size, m_shapeData->SliceCount, m_shapeData->StackCount);
		auto mc = GetComponent<MeshComponent_Info>();
		mc->m_assignedComponent->SetMesh(newMesh);
	}
}

void SphereShapeObject_Info::SetStackCount(SE_G::DeferredRenderer* renderSystem, uint32_t newStackCount) {
	if (newStackCount > 0) {
		m_shapeData->StackCount = newStackCount;
		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateSphereMesh(renderSystem->GetDevice(),
				m_shapeData->Size, m_shapeData->SliceCount, m_shapeData->StackCount);
		auto mc = GetComponent<MeshComponent_Info>();
		mc->m_assignedComponent->SetMesh(newMesh);
	}
}
