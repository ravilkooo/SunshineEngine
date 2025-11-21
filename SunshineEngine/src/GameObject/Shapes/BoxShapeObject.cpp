#include <GameObject/Shapes/BoxShapeObject.h>
#include <Graphics/Renderer/Technique/ColliderTechnique.h>
#include <Component/PhysicsComponent.h>

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
	auto tc_info = AddComponent<TransformComponent_Info>();
	tc_info->m_assignedComponent = eastl::make_unique<TransformComponent>(device);

	auto rc_info = AddComponent<RenderComponent_Info>();
	rc_info->m_assignedComponent = eastl::make_unique<RenderComponent>(renderSystem);

	//auto tc_info = GetComponent<TransformComponent_Info>();
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
	gBufferTech->m_mesh = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(
		device, obj->m_shapeData->Size);
	gBufferTech->SetTexture(MakeEngineAssetPath_Wstring(L"DefaultTexture.dds"));
	rc_info->AddTechnique(eastl::move(gBufferTech));

	// PhysicsComponent
	auto pc_info = obj->AddComponent<PhysicsComponent_Info>(rc_info.get(), tc_info.get());

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
		// Call SetMesh from RenderComponent
		auto rc = GetComponent<RenderComponent_Info>();
		rc->SetMesh(newMesh);
	}
}
