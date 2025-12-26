#include <GameObject/Shapes/BoxShapeObject.h>

#include <Graphics/Renderer/Technique/ColliderTechnique.h>
#include <Graphics/GraphicsResources/Texture.h>

#include <Component/PhysicsComponent.h>
#include <Component/MeshComponent.h>

BoxShapeObject_Info::BoxShapeObject_Info(SE::UUID uuid,
	SE_G::DeferredRenderer* renderSystem, BoxShapeData initData)
{
	auto device = renderSystem->GetDevice();
	m_UUID = uuid;
	m_group = GameObjectGroup::Shapes;
	m_type.m_asShape = ShapeObjectType::Box;
	m_name = "Box";
	m_shapeData = eastl::make_shared<BoxShapeData>(initData);

	// TransformComponent
	auto tc_info = AddComponent<TransformComponent_Info>(device);

	// RenderComponent and techniques
	auto rc_info = AddComponent<RenderComponent_Info>(m_UUID, renderSystem);

	// MeshComponent (holds shared mesh resource)
	auto newMesh = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(device, initData.Size);
	auto mesh_info = AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), m_UUID, newMesh);

	auto texture = eastl::make_shared<SE_G::Bind::Texture>(
		rc_info->GetDevice(),
		AssetPath(L"DefaultTexture.dds", AssetPath::AssetSource::Engine), 0u,
		SE_G::Bind::PipelineStage::PIXEL_SHADER);
	mesh_info->SetTexture(texture);
}

BoxShapeObject_Info::BoxShapeObject_Info(SE_G::DeferredRenderer* renderSystem, BoxShapeData initData) :
	BoxShapeObject_Info(SE::UUID(), renderSystem, initData)
{
}

eastl::unique_ptr<BoxShapeObject_Info> BoxShapeObject_Info::FromJson(
	SE_G::DeferredRenderer* renderSystem, const json& j)
{
	eastl::unique_ptr<BoxShapeObject_Info> obj = eastl::make_unique<BoxShapeObject_Info>();

	obj->m_UUID = SE::UUID(j["m_UUID"].get<uint64_t>());
	obj->m_shapeData = eastl::make_shared<BoxShapeData>(j["m_shapeData"].get<BoxShapeData>());
	obj->m_name = "Box";
	obj->m_group = GameObjectGroup::Shapes;
	obj->m_type.m_asShape = ShapeObjectType::Box;

	auto device = renderSystem->GetDevice();

	// TransformComponent
	auto tc_info = obj->AddComponent<TransformComponent_Info>(device);
	if (j["components"].contains("Transform")) {
		tc_info->FromJson(j["components"]["Transform"], device);
	}

	// RenderComponent and technique
	auto rc_info = obj->AddComponent<RenderComponent_Info>(obj->m_UUID, renderSystem);

	/*
	auto mc_info = obj->AddComponent<MeshComponent_Info>();
	mc_info->FromJson(j["components"]["Mesh"],
		device, rc_info.get(),
		tc_info.get(), obj->m_UUID);
	*/

	// MeshComponent (holds shared mesh resource)
	auto newMesh = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(device, obj->m_shapeData->Size);
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
			AssetPath(L"DefaultTexture.dds", AssetPath::AssetSource::Engine), 0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);
	
		mesh_info->SetTexture(texture);
	}

	return obj;
}

DXSM::Vector3 BoxShapeObject_Info::GetSize() {
	return m_shapeData->Size;
}

void BoxShapeObject_Info::SetSize(SE_G::DeferredRenderer* renderSystem, DXSM::Vector3 newSize) {
	if (DX::XMVector3Greater(newSize, DXSM::Vector3::Zero)) {
		m_shapeData->Size = newSize;

		eastl::shared_ptr<SE_G::Mesh> newMesh =
			SE_G::Mesh::CreateUnwrappedBoxMesh(renderSystem->GetDevice(),
				newSize);

		auto mc = GetComponent<MeshComponent_Info>();
		mc->m_assignedComponent->SetMesh(newMesh);
	}
}
