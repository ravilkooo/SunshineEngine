#include <GameObject/Shapes/CylinderShapeObject.h>
#include <Graphics/GraphicsResources/Texture.h>

#include <ResourceManager/ResourceManagerFacade.h>

CylinderShapeObject_Info::CylinderShapeObject_Info(SE::UUID uuid,
	SE_G::DeferredRenderer* renderSystem, CylinderShapeData initData)
{
	auto device = renderSystem->GetDevice();
	m_UUID = uuid;
	m_group = GameObjectGroup::Shapes;
	m_type.m_asShape = ShapeObjectType::Cylinder;
	m_name = "Cylinder";
	m_shapeData = eastl::make_shared<CylinderShapeData>(initData);
	m_shapeData->SliceCount = std::min(initData.SliceCount, MaxSliceCount);

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>(device);

	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

	eastl::shared_ptr<SE_G::Mesh> newMesh;
	AssetPath meshPath = AssetPath(L"Cylinder");
	meshPath.m_params.param1 = m_shapeData->SliceCount;
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	newMesh = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	newMesh->m_meshPath = meshRes->m_meshPath;

	auto mesh_info = AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), m_UUID, newMesh);

	AssetPath texPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	mesh_info->SetTexture(texture);
}

CylinderShapeObject_Info::CylinderShapeObject_Info(SE_G::DeferredRenderer* renderSystem, CylinderShapeData initData) :
	CylinderShapeObject_Info(SE::UUID(), renderSystem, initData)
{
}

eastl::unique_ptr<CylinderShapeObject_Info> CylinderShapeObject_Info::FromJson(
	SE_G::DeferredRenderer* renderSystem, const json& j)
{
	eastl::unique_ptr<CylinderShapeObject_Info> obj = eastl::make_unique<CylinderShapeObject_Info>();

	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	obj->m_shapeData = eastl::make_shared<CylinderShapeData>(j["m_shapeData"].get<CylinderShapeData>());
	obj->m_shapeData->SliceCount = std::min(obj->m_shapeData->SliceCount, MaxSliceCount);
	obj->m_name = "Cylinder";
	obj->m_group = GameObjectGroup::Shapes;
	obj->m_type.m_asShape = ShapeObjectType::Cylinder;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc_info = obj->AddComponent<TransformComponent_Info>(device);
	if (j["components"].contains("Transform")) {
		tc_info->FromJson(j["components"]["Transform"], device);
	}

	// RenderComponent and technique
	auto rc_info = obj->AddComponent<RenderComponent_Info>(obj->m_UUID, renderSystem);

	eastl::shared_ptr<SE_G::Mesh> newMesh;
	AssetPath meshPath = AssetPath(L"Cylinder");
	meshPath.m_params.param1 = std::min(obj->m_shapeData->SliceCount, MaxSliceCount);
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	newMesh = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	newMesh->m_meshPath = meshRes->m_meshPath;

	auto mesh_info = obj->AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), obj->m_UUID, newMesh);

	AssetPath texPath;
	if (j["components"]["Mesh"].contains("Texture"))
	{
		texPath.FromJson(j["components"]["Mesh"]["Texture"]);
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
		texHandle = rm.LoadByPath(ap);
	}

	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	mesh_info->SetTexture(texture);

	return obj;
}

uint32_t CylinderShapeObject_Info::GetSliceCount() {
	return m_shapeData->SliceCount;
}

void CylinderShapeObject_Info::SetSliceCount(SE_G::DeferredRenderer* renderSystem, uint32_t newSliceCount) {
	if (newSliceCount > 0) {
		m_shapeData->SliceCount = std::min(newSliceCount, MaxSliceCount);

		eastl::shared_ptr<SE_G::Mesh> newMesh;
		AssetPath meshPath = AssetPath(L"Cylinder");
		meshPath.m_params.param1 = m_shapeData->SliceCount;
		auto& rm = ResourceManagerFacade::Instance();
		ResourceHandle meshHandle = rm.LoadByPath(meshPath);
		SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
		newMesh = eastl::shared_ptr<SE_G::Mesh>(
			meshRes,
			[](SE_G::Mesh*) {}
		);
		newMesh->m_meshPath = meshRes->m_meshPath;

		auto mc = GetComponent<MeshComponent_Info>();
		mc->m_assignedComponent->SetMesh(newMesh);
	}
}
