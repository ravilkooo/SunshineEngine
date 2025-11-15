#include <GameObject/Shapes/ShapeObject.h>

BoxShapeObject_Info::BoxShapeObject_Info(SE::UUID uuid,
	SE_G::DeferredRenderer* renderSystem, BoxShapeData initData) :
	ShapeObject_Info<BoxShapeData>(uuid, renderSystem, initData)
{
	auto device = renderSystem->GetDevice();
	m_type.m_asShape = ShapeObjectType::Box;
	m_name = "Box";

	auto rc_info = AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);

	auto tc_info = GetComponent<TransformComponent_Info>();
	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
		renderSystem, tc_info->m_assignedComponent.get(), "GPass", m_UUID);
	gBufferTech->m_mesh = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(
		device, initData.Size);
	gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultTexture.dds"));

	rc_info->AddTechnique(eastl::move(gBufferTech));
}

BoxShapeObject_Info::BoxShapeObject_Info(SE_G::DeferredRenderer* renderSystem, BoxShapeData initData) :
	BoxShapeObject_Info(SE::UUID(), renderSystem, initData)
{
}

DXSM::Vector3 BoxShapeObject_Info::GetSize() {
	return m_shapeData.Size;
}

void BoxShapeObject_Info::SetSize(SE_G::DeferredRenderer* renderSystem, DXSM::Vector3 newSize) {
	if (DX::XMVector3Greater(newSize, DXSM::Vector3::Zero)) {
		m_shapeData.Size = newSize;

		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateUnwrappedBoxMesh(renderSystem->GetDevice(),
				newSize);
		// Call SetMesh from RenderComponent
		auto rc = GetComponent<RenderComponent_Info>();
		rc->SetMesh(newMesh);
	}
}

SphereShapeObject_Info::SphereShapeObject_Info(SE::UUID uuid,
	SE_G::DeferredRenderer* renderSystem, SphereShapeData initData) :
	ShapeObject_Info<SphereShapeData>(uuid, renderSystem, initData)
{
	auto device = renderSystem->GetDevice();
	m_type.m_asShape = ShapeObjectType::Sphere;
	m_name = "Sphere";

	auto rc_info = AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);

	auto tc_info = GetComponent<TransformComponent_Info>();
	auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
		renderSystem, tc_info->m_assignedComponent.get(), "GPass", m_UUID);
	gBufferTech->m_mesh = SE_G::Mesh::CreateSphereMesh(
		device, initData.Size, initData.SliceCount, initData.StackCount);
	gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultTexture.dds"));

	rc_info->AddTechnique(eastl::move(gBufferTech));
}

SphereShapeObject_Info::SphereShapeObject_Info(SE_G::DeferredRenderer* renderSystem, SphereShapeData initData) :
	SphereShapeObject_Info(SE::UUID(), renderSystem, initData)
{
}

DXSM::Vector3 SphereShapeObject_Info::GetSize() {
	return m_shapeData.Size;
}

uint32_t SphereShapeObject_Info::GetSliceCount() {
	return m_shapeData.SliceCount;
}

uint32_t SphereShapeObject_Info::GetStackCount() {
	return m_shapeData.StackCount;
}

void SphereShapeObject_Info::SetSize(SE_G::DeferredRenderer* renderSystem, DXSM::Vector3 newSize) {
	if (DX::XMVector3Greater(newSize, DXSM::Vector3::Zero)) {
		m_shapeData.Size = newSize;

		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateSphereMesh(renderSystem->GetDevice(),
				newSize, m_shapeData.SliceCount, m_shapeData.StackCount);
		auto rc = GetComponent<RenderComponent_Info>();
		rc->SetMesh(newMesh);
	}
}

void SphereShapeObject_Info::SetSliceCount(SE_G::DeferredRenderer* renderSystem, uint32_t newSliceCount) {
	if (newSliceCount > 0) {
		m_shapeData.SliceCount = newSliceCount;
		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateSphereMesh(renderSystem->GetDevice(),
				m_shapeData.Size, m_shapeData.SliceCount, m_shapeData.StackCount);
		auto rc = GetComponent<RenderComponent_Info>();
		rc->SetMesh(newMesh);
	}
}

void SphereShapeObject_Info::SetStackCount(SE_G::DeferredRenderer* renderSystem, uint32_t newStackCount) {
	if (newStackCount > 0) {
		m_shapeData.StackCount = newStackCount;
		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateSphereMesh(renderSystem->GetDevice(),
				m_shapeData.Size, m_shapeData.SliceCount, m_shapeData.StackCount);
		auto rc = GetComponent<RenderComponent_Info>();
		rc->SetMesh(newMesh);
	}
}

GeosphereShapeObject_Info::GeosphereShapeObject_Info(SE::UUID uuid,
	SE_G::DeferredRenderer* renderSystem, GeosphereShapeData initData) :
	ShapeObject_Info<GeosphereShapeData>(uuid, renderSystem, initData)
{
	auto device = renderSystem->GetDevice();
	m_type.m_asShape = ShapeObjectType::Geosphere;
	m_name = "Geosphere";

	auto rc_info = AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_shared<RenderComponent>(renderSystem);

	auto tc_info = GetComponent<TransformComponent_Info>();
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

DXSM::Vector3 GeosphereShapeObject_Info::GetSize() {
	return m_shapeData.Size;
}

uint32_t GeosphereShapeObject_Info::GetNumSubdivisions() {
	return m_shapeData.NumSubdivisions;
}

void GeosphereShapeObject_Info::SetSize(SE_G::DeferredRenderer* renderSystem, DXSM::Vector3 newSize) {
	if (DX::XMVector3Greater(newSize, DXSM::Vector3::Zero)) {
		m_shapeData.Size = newSize;
		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateGeosphereMesh(renderSystem->GetDevice(),
				newSize, static_cast<UINT>(m_shapeData.NumSubdivisions));
		auto rc = GetComponent<RenderComponent_Info>();
		rc->SetMesh(newMesh);
	}
}

void GeosphereShapeObject_Info::SetNumSubdivisions(SE_G::DeferredRenderer* renderSystem, uint32_t newNumSubdivisions) {
	m_shapeData.NumSubdivisions = newNumSubdivisions;
	eastl::shared_ptr<SE_G::Mesh> newMesh =
		SE_G::Mesh::CreateGeosphereMesh(renderSystem->GetDevice(),
			m_shapeData.Size, static_cast<UINT>(m_shapeData.NumSubdivisions));
	auto rc = GetComponent<RenderComponent_Info>();
	rc->SetMesh(newMesh);
}
