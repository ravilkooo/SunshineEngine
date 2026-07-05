#include <GameObject/Shapes/PlaneShapeObject.h>

#include <Graphics/Renderer/Technique/ColliderTechnique.h>
#include <Graphics/GraphicsResources/Texture.h>
#include <Graphics/GraphicsResources/Mesh.h>

#include <Component/PhysicsComponent.h>
#include <Component/MeshComponent.h>

#include <Serialization/GraphicsSerialization.h>
#include <Serialization/ShapeSerialization.h>
#include <Serialization/DXSMSerialization.h>

#include <ResourceManager/ResourceManagerFacade.h>

PlaneShapeObject_Info::PlaneShapeObject_Info(SE::UUID uuid,
	SE_G::DeferredRenderer* renderSystem, PlaneShapeData initData)
{
	auto device = renderSystem->GetDevice();
	m_UUID = uuid;
	m_group = GameObjectGroup::Shapes;
	m_type.m_asShape = ShapeObjectType::Plane;
	m_name = "Plane";
	m_shapeData = eastl::make_shared<PlaneShapeData>(initData);

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>(device);

	// RenderComponent and techniques
	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

	// MeshComponent (holds shared mesh resource)
	eastl::shared_ptr<SE_G::Mesh> newMesh;
	AssetPath meshPath = AssetPath(L"Plane");
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

PlaneShapeObject_Info::PlaneShapeObject_Info(SE_G::DeferredRenderer* renderSystem, PlaneShapeData initData) :
	PlaneShapeObject_Info(SE::UUID(), renderSystem, initData)
{
}

eastl::unique_ptr<PlaneShapeObject_Info> PlaneShapeObject_Info::FromJson(
	SE_G::DeferredRenderer* renderSystem, const json& j)
{
	eastl::unique_ptr<PlaneShapeObject_Info> obj = eastl::make_unique<PlaneShapeObject_Info>();

	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	obj->m_shapeData = eastl::make_shared<PlaneShapeData>(j["m_shapeData"].get<PlaneShapeData>());
	obj->m_name = "Plane";
	obj->m_group = GameObjectGroup::Shapes;
	obj->m_type.m_asShape = ShapeObjectType::Plane;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc_info = obj->AddComponent<TransformComponent_Info>(device);
	if (j["components"].contains("Transform")) {
		tc_info->FromJson(j["components"]["Transform"], device);
	}

	// RenderComponent and technique
	auto rc_info = obj->AddComponent<RenderComponent_Info>(obj->m_UUID, renderSystem);
	if (j["components"].contains("Render")) {
		rc_info->FromJson(j["components"]["Render"]);
	}

	// MeshComponent (holds shared mesh resource)
	eastl::shared_ptr<SE_G::Mesh> newMesh;
	AssetPath meshPath = AssetPath(L"Plane");
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
		texHandle = ResourceManagerFacade::Instance().LoadByPath(ap);
	}
	SE_G::Bind::Texture* texRes = rm.Get<SE_G::Bind::Texture>(texHandle);

	auto texture = eastl::shared_ptr<SE_G::Bind::Texture>(
		texRes,
		[](SE_G::Bind::Texture*) { /* do nothing, ResourceManager releases */ });
	mesh_info->SetTexture(texture);

	return obj;
}
