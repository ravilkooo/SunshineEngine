#include <GameObject/Shapes/GeosphereShapeObject.h>
#include <Graphics/GraphicsResources/Texture.h>

#include <ResourceManager/ResourceManagerFacade.h>

GeosphereShapeObject_Info::GeosphereShapeObject_Info(SE::UUID uuid,
	SE_G::DeferredRenderer* renderSystem, GeosphereShapeData initData)
{
	auto device = renderSystem->GetDevice();
	m_UUID = uuid;
	m_group = GameObjectGroup::Shapes;
	m_type.m_asShape = ShapeObjectType::Geosphere;
	m_name = "Geosphere";
	m_shapeData = eastl::make_shared<GeosphereShapeData>(initData);
	m_shapeData->NumSubdivisions = std::min<uint32_t>(std::max<uint32_t>(m_shapeData->NumSubdivisions, 0), 5);

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>(device);

	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

	auto mesh = SE_G::Mesh::CreateGeosphereMesh(
		device, static_cast<UINT>(initData.NumSubdivisions));
	auto mesh_info = AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), m_UUID, mesh);

	/*
	auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		rc_info->GetDevice(),
		AssetPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine), 0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER);
	mesh_info->SetTexture(texture);
	*/
	auto& rm = ResourceManagerFacade::Instance();
	AssetPath texPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	if (texRes)
	{
		auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
			texRes,
			[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
		mesh_info->SetTexture(texture);
	}
	else
	{
		auto texture = eastl::make_shared<SE_G::Bind::Texture>(
			renderSystem->GetDevice(), texPath, 0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);
		mesh_info->SetTexture(texture);
	}
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
	obj->m_shapeData->NumSubdivisions = std::min<uint32_t>(std::max<uint32_t>(obj->m_shapeData->NumSubdivisions, 0), 5);
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
		device,
		obj->m_shapeData->NumSubdivisions);
	auto mesh_info = obj->AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), obj->m_UUID, newMesh);

	AssetPath texPath;
	if (j["components"]["Mesh"].contains("Texture"))
	{
		texPath.FromJson(j["components"]["Mesh"]["Texture"]);
	}
	else {
		texPath = AssetPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine);
	}

	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	if (texRes)
	{
		auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
			texRes,
			[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
		mesh_info->SetTexture(texture);
	}
	else
	{
		auto texture = eastl::make_shared<SE_G::Bind::Texture>(
			renderSystem->GetDevice(), texPath, 0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);
		mesh_info->SetTexture(texture);
	}

	return obj;
}

uint32_t GeosphereShapeObject_Info::GetNumSubdivisions() {
	return m_shapeData->NumSubdivisions;
}

void GeosphereShapeObject_Info::SetNumSubdivisions(SE_G::DeferredRenderer* renderSystem, uint32_t newNumSubdivisions) {
	if (newNumSubdivisions > 0) {
		m_shapeData->NumSubdivisions = std::min<uint32_t>(std::max<uint32_t>(newNumSubdivisions, 0), 5);
		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateGeosphereMesh(renderSystem->GetDevice(),
				m_shapeData->NumSubdivisions);

		auto mc = GetComponent<MeshComponent_Info>();
		mc->m_assignedComponent->SetMesh(newMesh);
	}
}
