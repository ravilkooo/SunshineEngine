#include <GameObject/Shapes/GeosphereShapeObject.h>
#include <Graphics/GraphicsResources/Texture.h>

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
	auto tc_info = AddComponent<TransformComponent_Info>(device);

	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

	auto mesh = SE_G::Mesh::CreateGeosphereMesh(
		device, initData.Size, static_cast<UINT>(initData.NumSubdivisions));
	auto mesh_info = AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), m_UUID, mesh);

	auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		rc_info->GetDevice(),
		AssetPath(L"DefaultSphereTexture.dds", AssetPath::AssetSource::Engine), 0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER);
	mesh_info->SetTexture(texture);
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
	auto tc_info = obj->AddComponent<TransformComponent_Info>(device);
	if (j["components"].contains("Transform")) {
		tc_info->FromJson(j["components"]["Transform"], device);
	}

	// RenderComponent and technique
	auto rc_info = obj->AddComponent<RenderComponent_Info>(obj->m_UUID, renderSystem);

	auto newMesh = SE_G::Mesh::CreateGeosphereMesh(
		device, obj->m_shapeData->Size,
		static_cast<UINT>(obj->m_shapeData->NumSubdivisions));
	auto mesh_info = obj->AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), obj->m_UUID, newMesh);

	if (j["components"]["Mesh"].contains("Texture"))
	{
		AssetPath texPath;
		texPath.FromJson(j["components"]["Mesh"]["Texture"]);

		auto texture = eastl::make_shared<SE_G::Bind::Texture>(
			device, texPath, 0u, SE_G::Bind::PipelineStage::PIXEL_SHADER);

		mesh_info->SetTexture(texture);
	}
	else {
		auto texture = eastl::make_shared<SE_G::Bind::Texture>(
			device,
			AssetPath(L"DefaultSphereTexture.dds", AssetPath::AssetSource::Engine), 0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);

		mesh_info->SetTexture(texture);
	}

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

		auto mc = GetComponent<MeshComponent_Info>();
		mc->m_assignedComponent->SetMesh(newMesh);
	}
}

void GeosphereShapeObject_Info::SetNumSubdivisions(SE_G::DeferredRenderer* renderSystem, uint32_t newNumSubdivisions) {
	m_shapeData->NumSubdivisions = newNumSubdivisions;
	eastl::shared_ptr<SE_G::Mesh> newMesh =
		SE_G::Mesh::CreateGeosphereMesh(renderSystem->GetDevice(),
			m_shapeData->Size, static_cast<UINT>(m_shapeData->NumSubdivisions));

	auto mc = GetComponent<MeshComponent_Info>();
	mc->m_assignedComponent->SetMesh(newMesh);
}
