#pragma once

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>

#include <Component/TransformComponent.h>
#include <Component/RenderComponent.h>

#include <Graphics/Renderer/Technique/GPassTechnique.h>
#include <Graphics/Renderer/DeferredRenderer.h>

#include <GameObject/GameObject.h>
#include <GameObject/Shapes/ShapeData.h>

#include <Utils/StringUtils.h>
#include <Utils/UUID.h>

#include <Serialization/ShapeSerialization.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

template <class T>
class ShapeObject_Info : public GameObject_Info {
protected:
	T m_shapeData;

public:
	ShapeObject_Info(SE::UUID uuid,
		SE_G::DeferredRenderer* renderSystem, T initData) :
		GameObject_Info(uuid), m_shapeData(initData)
	{
		auto device = renderSystem->GetDevice();
		m_group = GameObjectGroup::Shapes;

		// TransformComponent
		auto tr_info = AddComponent<TransformComponent_Info>();
		tr_info->m_assignedComponent = eastl::make_shared<TransformComponent>(device);
	}

	ShapeObject_Info(SE_G::DeferredRenderer* renderSystem, T initData) :
		ShapeObject_Info(SE::UUID(), renderSystem, initData)
	{
	}

	json ToJson() const override  {
		json j = GameObject_Info::ToJson();
		j["m_shapeData"] = m_shapeData;
		return j;
	}
};

class BoxShapeObject_Info :
	public ShapeObject_Info<BoxShapeData>
{
public:
	BoxShapeObject_Info(SE::UUID uuid,
		SE_G::DeferredRenderer* renderSystem, BoxShapeData initData);

	BoxShapeObject_Info(SE_G::DeferredRenderer* renderSystem, BoxShapeData initData);

	DXSM::Vector3 GetSize();
	void SetSize(SE_G::DeferredRenderer* renderSystem, DXSM::Vector3 newSize);
};

class SphereShapeObject_Info :
	public ShapeObject_Info<SphereShapeData>
{
public:
	SphereShapeObject_Info(SE::UUID uuid,
		SE_G::DeferredRenderer* renderSystem, SphereShapeData initData);

	SphereShapeObject_Info(SE_G::DeferredRenderer* renderSystem, SphereShapeData initData);

	DXSM::Vector3 GetSize();
	uint32_t GetSliceCount();
	uint32_t GetStackCount();

	void SetSize(SE_G::DeferredRenderer* renderSystem, DXSM::Vector3 newSize);
	void SetSliceCount(SE_G::DeferredRenderer* renderSystem, uint32_t newSliceCount);
	void SetStackCount(SE_G::DeferredRenderer* renderSystem, uint32_t newStackCount);
};

class GeosphereShapeObject_Info :
	public ShapeObject_Info<GeosphereShapeData>
{
public:
	GeosphereShapeObject_Info(SE::UUID uuid,
		SE_G::DeferredRenderer* renderSystem, GeosphereShapeData initData);

	GeosphereShapeObject_Info(SE_G::DeferredRenderer* renderSystem, GeosphereShapeData initData);

	DXSM::Vector3 GetSize();
	uint32_t GetNumSubdivisions();

	void SetSize(SE_G::DeferredRenderer* renderSystem, DXSM::Vector3 newSize);
	void SetNumSubdivisions(SE_G::DeferredRenderer* renderSystem, uint32_t newNumSubdivisions);
};
