#include <GameObject/Shapes/GeosphereShapeObject.h>

GeosphereShapeObject_Info::GeosphereShapeObject_Info(SE::UUID uuid,
	SE_G::DeferredRenderer* renderSystem, GeosphereShapeData initData)
{
	auto device = renderSystem->GetDevice();
	m_UUID = uuid;
	m_group = GameObjectGroup::Shapes;
	m_type.m_asShape = ShapeObjectType::Geosphere;
	m_name = "Geosphere";
	m_shapeData = eastl::make_shared<GeosphereShapeData>(initData);

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>();
	tc_info->m_assignedComponent = eastl::make_unique<TransformComponent>(device);

	auto rc_info = AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_unique<RenderComponent>(renderSystem);

	//auto tc_info = GetComponent<TransformComponent_Info>();
	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
		renderSystem, tc_info->m_assignedComponent.get(), "GPass", m_UUID);
	gBufferTech->m_mesh = SE_G::Mesh::CreateGeosphereMesh(
		device, initData.Size, static_cast<UINT>(initData.NumSubdivisions));
	gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultTexture.dds"));

	rc_info->AddTechnique(eastl::move(gBufferTech));
}

GeosphereShapeObject_Info::GeosphereShapeObject_Info(SE_G::DeferredRenderer* renderSystem, GeosphereShapeData initData) :
	GeosphereShapeObject_Info(SE::UUID(), renderSystem, initData)
{
}

eastl::unique_ptr<GeosphereShapeObject_Info> GeosphereShapeObject_Info::FromJson(
	SE_G::DeferredRenderer* renderSystem, const json& j)
{
	eastl::unique_ptr<GeosphereShapeObject_Info> obj = eastl::make_unique<GeosphereShapeObject_Info>();

	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	obj->m_shapeData = eastl::make_shared<GeosphereShapeData>(j["m_shapeData"].get<GeosphereShapeData>());
	obj->m_name = "Geosphere";
	obj->m_group = GameObjectGroup::Shapes;
	obj->m_type.m_asShape = ShapeObjectType::Geosphere;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc_info = obj->AddComponent<TransformComponent_Info>();
	if (j["components"].contains("Transform")) {
		tc_info->FromJson(j["components"]["Transform"], device);
	}
	else {
		tc_info->m_assignedComponent = eastl::make_unique<TransformComponent>(device);
	}

	// RenderComponent and technique
	auto rc_info = obj->AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_unique<RenderComponent>(renderSystem);

	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
		renderSystem, tc_info->m_assignedComponent.get(), "GPass", obj->m_UUID);
	gBufferTech->m_mesh = SE_G::Mesh::CreateGeosphereMesh(
		device, obj->m_shapeData->Size, static_cast<UINT>(obj->m_shapeData->NumSubdivisions));
	gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultTexture.dds"));

	rc_info->AddTechnique(eastl::move(gBufferTech));

	return obj;
}

DXSM::Vector3 GeosphereShapeObject_Info::GetSize() {
	return m_shapeData->Size;
}

uint32_t GeosphereShapeObject_Info::GetNumSubdivisions() {
	return m_shapeData->NumSubdivisions;
}

void GeosphereShapeObject_Info::SetSize(SE_G::DeferredRenderer* renderSystem, DXSM::Vector3 newSize) {
	if (DX::XMVector3Greater(newSize, DXSM::Vector3::Zero)) {
		m_shapeData->Size = newSize;
		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateGeosphereMesh(renderSystem->GetDevice(),
				newSize, static_cast<UINT>(m_shapeData->NumSubdivisions));
		auto rc = GetComponent<RenderComponent_Info>();
		rc->SetMesh(newMesh);
	}
}

void GeosphereShapeObject_Info::SetNumSubdivisions(SE_G::DeferredRenderer* renderSystem, uint32_t newNumSubdivisions) {
	m_shapeData->NumSubdivisions = newNumSubdivisions;
	eastl::shared_ptr<SE_G::Mesh> newMesh =
		SE_G::Mesh::CreateGeosphereMesh(renderSystem->GetDevice(),
			m_shapeData->Size, static_cast<UINT>(m_shapeData->NumSubdivisions));
	auto rc = GetComponent<RenderComponent_Info>();
	rc->SetMesh(newMesh);
}
