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

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>(device);

	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

	auto mesh = SE_G::Mesh::CreateCylinderMesh(
		device, initData.Radius, initData.Height, static_cast<UINT>(initData.SliceCount));
	auto mesh_info = AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), m_UUID, mesh);

	auto& rm = ResourceManagerFacade::Instance();
	AssetPath texPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
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

	auto newMesh = SE_G::Mesh::CreateCylinderMesh(
		device, obj->m_shapeData->Radius, obj->m_shapeData->Height,
		obj->m_shapeData->SliceCount);
	auto mesh_info = obj->AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), obj->m_UUID, newMesh);

	AssetPath texPath;
	if (j["components"]["Mesh"].contains("Texture"))
	{
		texPath.FromJson(j["components"]["Mesh"]["Texture"]);
	}
	else {
		texPath = AssetPath(L"Textures/DefaultTexture.dds", AssetPath::AssetSource::Engine);
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

float CylinderShapeObject_Info::GetRadius() {
	return m_shapeData->Radius;
}

float CylinderShapeObject_Info::GetHeight() {
	return m_shapeData->Height;
}

uint32_t CylinderShapeObject_Info::GetSliceCount() {
	return m_shapeData->SliceCount;
}

void CylinderShapeObject_Info::SetRadius(SE_G::DeferredRenderer* renderSystem, float newRadius) {
	if (newRadius > 0) {
		m_shapeData->Radius = newRadius;
		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateCylinderMesh(renderSystem->GetDevice(),
				newRadius, m_shapeData->Height, m_shapeData->SliceCount);

		auto mc = GetComponent<MeshComponent_Info>();
		mc->m_assignedComponent->SetMesh(newMesh);
	}
}

void CylinderShapeObject_Info::SetHeight(SE_G::DeferredRenderer* renderSystem, float newHeight) {
	if (newHeight > 0) {
		m_shapeData->Height = newHeight;
		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateCylinderMesh(renderSystem->GetDevice(),
				m_shapeData->Radius, newHeight, m_shapeData->SliceCount);

		auto mc = GetComponent<MeshComponent_Info>();
		mc->m_assignedComponent->SetMesh(newMesh);
	}
}

void CylinderShapeObject_Info::SetSliceCount(SE_G::DeferredRenderer* renderSystem, uint32_t newSliceCount) {
	if (newSliceCount > 0) {
		m_shapeData->SliceCount = newSliceCount;
		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateCylinderMesh(renderSystem->GetDevice(),
				m_shapeData->Radius, m_shapeData->Height, m_shapeData->SliceCount);
		auto mc = GetComponent<MeshComponent_Info>();
		mc->m_assignedComponent->SetMesh(newMesh);
	}
}
