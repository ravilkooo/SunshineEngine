#pragma once

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <GameObject/Shapes/ShapeObject.h>
#include <GameObject/Shapes/ShapeData.h>

#include <Serialization/ShapeSerialization.h>
#include <Serialization/DXSMSerialization.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class CylinderShapeObject_Info :
	public ShapeObject_Info<CylinderShapeData>
{
public:

	CylinderShapeObject_Info() {};
	explicit CylinderShapeObject_Info(SE::UUID uuid,
		SE_G::DeferredRenderer* renderSystem, CylinderShapeData initData = CylinderShapeData());
	explicit CylinderShapeObject_Info(
		SE_G::DeferredRenderer* renderSystem, CylinderShapeData initData = CylinderShapeData());

	static eastl::unique_ptr<CylinderShapeObject_Info> FromJson(
		SE_G::DeferredRenderer* renderSystem,
		const json& j);

	float GetRadius();
	float GetHeight();
	uint32_t GetSliceCount();

	void SetRadius(SE_G::DeferredRenderer* renderSystem, float newRadius);
	void SetHeight(SE_G::DeferredRenderer* renderSystem, float newHeight);
	void SetSliceCount(SE_G::DeferredRenderer* renderSystem, uint32_t newSliceCount);

};
