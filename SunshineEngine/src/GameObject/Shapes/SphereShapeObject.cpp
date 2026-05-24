#include <GameObject/Shapes/SphereShapeObject.h>

#include <Component/PhysicsComponent.h>
#include <Component/MeshComponent.h>

#include <Serialization/GraphicsSerialization.h>

#include <Graphics/Renderer/Technique/ColliderTechnique.h>
#include <Graphics/GraphicsResources/Texture.h>

#include <ResourceManager/ResourceManagerFacade.h>

SphereShapeObject_Info::SphereShapeObject_Info(SE::UUID uuid,
	SE_G::DeferredRenderer* renderSystem, SphereShapeData initData)
{
	auto device = renderSystem->GetDevice();
	m_UUID = uuid;
	m_group = GameObjectGroup::Shapes;
	m_type.m_asShape = ShapeObjectType::Sphere;
	m_name = "Sphere";
	m_shapeData = eastl::make_shared<SphereShapeData>(initData);
	m_shapeData->SliceCount= std::min(m_shapeData->SliceCount, MaxSliceCount);
	m_shapeData->StackCount = std::min(m_shapeData->StackCount, MaxStackCount);
	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>(device);

	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

	eastl::shared_ptr<SE_G::Mesh> newMesh;
	AssetPath meshPath = AssetPath(L"Sphere");
	meshPath.m_params.asMesh.param1 = m_shapeData->SliceCount;
	meshPath.m_params.asMesh.param2 = m_shapeData->StackCount;
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	newMesh = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	newMesh->m_meshPath = meshRes->m_meshPath;

	auto mesh_info = AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), m_UUID, newMesh);

	AssetPath texPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine);
	ResourceHandle texHandle = rm.LoadByPath(texPath);
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
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
	obj->m_name = "Sphere";
	obj->m_group = GameObjectGroup::Shapes;
	obj->m_type.m_asShape = ShapeObjectType::Sphere;
	obj->m_shapeData = eastl::make_shared<SphereShapeData>(j["m_shapeData"].get<SphereShapeData>());
	obj->m_shapeData->SliceCount = std::min(obj->m_shapeData->SliceCount, MaxSliceCount);
	obj->m_shapeData->StackCount = std::min(obj->m_shapeData->StackCount, MaxStackCount);

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc_info = obj->AddComponent<TransformComponent_Info>(device);
	if (j["components"].contains("Transform")) {
		tc_info->FromJson(j["components"]["Transform"], device);
	}

	// RenderComponent and technique
	auto rc_info = obj->AddComponent<RenderComponent_Info>(obj->m_UUID, renderSystem);

	eastl::shared_ptr<SE_G::Mesh> newMesh;
	AssetPath meshPath = AssetPath(L"Sphere");
	meshPath.m_params.asMesh.param1 = obj->m_shapeData->SliceCount;
	meshPath.m_params.asMesh.param2 = obj->m_shapeData->StackCount;
	auto& rm = ResourceManagerFacade::Instance();
	ResourceHandle meshHandle = rm.LoadByPath(meshPath);
	SE_G::Mesh* meshRes = rm.Get<SE_G::Mesh>(meshHandle);
	newMesh = eastl::shared_ptr<SE_G::Mesh>(
		meshRes,
		[](SE_G::Mesh*) {}
	);
	newMesh->m_meshPath = meshRes->m_meshPath;

	auto mesh_info = obj->AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), obj->m_UUID, newMesh);
	if (j["components"]["Mesh"].contains("m_cullMode"))
	{
		D3D11_CULL_MODE cullMode = D3D11_CULL_BACK;
		j["components"]["Mesh"].at("m_cullMode").get_to(cullMode);
		mesh_info->SetCullMode(cullMode);
	}

	AssetPath texPath(L"");
	if (j["components"]["Mesh"].contains("Texture"))
	{
		texPath.FromJson(j["components"]["Mesh"]["Texture"]);
	}
	else {
		texPath = AssetPath(L"Textures/DefaultSphereTexture.dds", AssetPath::AssetSource::Engine);
	}

	ResourceHandle texHandle = rm.LoadByPath(texPath);
	if (texHandle.guid == 0) {
		// Error
		auto ap = AssetPath(
			SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
			AssetPath::AssetSource::Engine);
		texHandle = ResourceManagerFacade::Instance().LoadByPath(ap);
	}
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);
	
	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	mesh_info->SetTexture(texture);

	return obj;
}

uint32_t SphereShapeObject_Info::GetSliceCount() {
	return m_shapeData->SliceCount;
}

uint32_t SphereShapeObject_Info::GetStackCount() {
	return m_shapeData->StackCount;
}

void SphereShapeObject_Info::SetSliceCount(SE_G::DeferredRenderer* renderSystem, uint32_t newSliceCount) {
	if (newSliceCount > 0) {
		m_shapeData->SliceCount = std::min(newSliceCount, MaxSliceCount);

		eastl::shared_ptr<SE_G::Mesh> newMesh;
		AssetPath meshPath = AssetPath(L"Sphere");
		meshPath.m_params.asMesh.param1 = m_shapeData->SliceCount;
		meshPath.m_params.asMesh.param2 = m_shapeData->StackCount;
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

void SphereShapeObject_Info::SetStackCount(SE_G::DeferredRenderer* renderSystem, uint32_t newStackCount) {
	if (newStackCount > 0) {
		m_shapeData->StackCount = std::min(newStackCount, MaxStackCount);

		eastl::shared_ptr<SE_G::Mesh> newMesh;
		AssetPath meshPath = AssetPath(L"Sphere");
		meshPath.m_params.asMesh.param1 = m_shapeData->SliceCount;
		meshPath.m_params.asMesh.param2 = m_shapeData->StackCount;
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
