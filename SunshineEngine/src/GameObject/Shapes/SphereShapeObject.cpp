#include <GameObject/Shapes/SphereShapeObject.h>

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
	auto tc_info = AddComponent<TransformComponent_Info>();
	tc_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);

	auto rc_info = AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);

	//auto tc_info = GetComponent<TransformComponent_Info>();
	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
		renderSystem, tc_info->m_assignedComponent.get(), "GPass", m_UUID);
	gBufferTech->m_mesh = SE_G::Mesh::CreateSphereMesh(
		device, initData.Size, initData.SliceCount, initData.StackCount);
	gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultSphereTexture.dds"));

	rc_info->AddTechnique(eastl::move(gBufferTech));
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
	auto tc_info = obj->AddComponent<TransformComponent_Info>();
	if (j["components"].contains("Transform")) {
		tc_info->FromJson(j["components"]["Transform"], device);
	}
	else {
		tc_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);
	}

	// RenderComponent and technique
	auto rc_info = obj->AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);

	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
		renderSystem, tc_info->m_assignedComponent.get(), "GPass", obj->m_UUID);
	gBufferTech->m_mesh = SE_G::Mesh::CreateSphereMesh(
		device, obj->m_shapeData->Size, obj->m_shapeData->SliceCount, obj->m_shapeData->StackCount);
	gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultSphereTexture.dds"));

	rc_info->AddTechnique(eastl::move(gBufferTech));

	return eastl::move(obj);
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
		auto rc = GetComponent<RenderComponent_Info>();
		rc->SetMesh(newMesh);
	}
}

void SphereShapeObject_Info::SetSliceCount(SE_G::DeferredRenderer* renderSystem, uint32_t newSliceCount) {
	if (newSliceCount > 0) {
		m_shapeData->SliceCount = newSliceCount;
		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateSphereMesh(renderSystem->GetDevice(),
				m_shapeData->Size, m_shapeData->SliceCount, m_shapeData->StackCount);
		auto rc = GetComponent<RenderComponent_Info>();
		rc->SetMesh(newMesh);
	}
}

void SphereShapeObject_Info::SetStackCount(SE_G::DeferredRenderer* renderSystem, uint32_t newStackCount) {
	if (newStackCount > 0) {
		m_shapeData->StackCount = newStackCount;
		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateSphereMesh(renderSystem->GetDevice(),
				m_shapeData->Size, m_shapeData->SliceCount, m_shapeData->StackCount);
		auto rc = GetComponent<RenderComponent_Info>();
		rc->SetMesh(newMesh);
	}
}
