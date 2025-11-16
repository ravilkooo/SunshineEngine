#pragma once

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <GameObject/Shapes/ShapeObject.h>
#include <GameObject/Shapes/ShapeData.h>

#include <Serialization/ShapeSerialization.h>
#include <Serialization/DXSMSerialization.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class GeosphereShapeObject_Info :
	public ShapeObject_Info<GeosphereShapeData>
{
public:
	
	GeosphereShapeObject_Info() {};
	explicit GeosphereShapeObject_Info(SE::UUID uuid,
		SE_G::DeferredRenderer* renderSystem, GeosphereShapeData initData = GeosphereShapeData());
	explicit GeosphereShapeObject_Info(
		SE_G::DeferredRenderer* renderSystem, GeosphereShapeData initData = GeosphereShapeData());
	
	static eastl::unique_ptr<GeosphereShapeObject_Info> FromJson(
		SE_G::DeferredRenderer* renderSystem,
		const json& j);

	DXSM::Vector3 GetSize();
	uint32_t GetNumSubdivisions();

	void SetSize(SE_G::DeferredRenderer* renderSystem, DXSM::Vector3 newSize);
	void SetNumSubdivisions(SE_G::DeferredRenderer* renderSystem, uint32_t newNumSubdivisions);

};
