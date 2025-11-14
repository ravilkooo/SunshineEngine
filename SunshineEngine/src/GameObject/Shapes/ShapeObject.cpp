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
		device, initData.Size.x, initData.Size.y, initData.Size.z);
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

void BoxShapeObject_Info::SetSize(DXSM::Vector3 newSize) {
	if (DX::XMVector3Greater(newSize, DXSM::Vector3::Zero)) {
		m_shapeData.Size = newSize;

		// Call SetMesh from RenderComponent

	}
}