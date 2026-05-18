#pragma once

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <GameObject/Shapes/ShapeObject.h>
#include <GameObject/Shapes/ShapeData.h>

#include <Serialization/ShapeSerialization.h>
#include <Serialization/DXSMSerialization.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class SphereShapeObject_Info :
	public ShapeObject_Info<SphereShapeData>
{
	static const uint32_t MaxSliceCount = 40;
	static const uint32_t MaxStackCount = 40;
public:

	SphereShapeObject_Info() {};
	explicit SphereShapeObject_Info(SE::UUID uuid,
		SE_G::DeferredRenderer* renderSystem, SphereShapeData initData = SphereShapeData());
	explicit SphereShapeObject_Info(
		SE_G::DeferredRenderer* renderSystem, SphereShapeData initData = SphereShapeData());
	
	static eastl::unique_ptr<SphereShapeObject_Info> FromJson(
		SE_G::DeferredRenderer* renderSystem,
		const json& j);

	uint32_t GetSliceCount();
	uint32_t GetStackCount();

	void SetSliceCount(SE_G::DeferredRenderer* renderSystem, uint32_t newSliceCount);
	void SetStackCount(SE_G::DeferredRenderer* renderSystem, uint32_t newStackCount);
};