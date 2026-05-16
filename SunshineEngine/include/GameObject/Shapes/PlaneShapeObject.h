#pragma once

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <GameObject/Shapes/ShapeObject.h>
#include <GameObject/Shapes/ShapeData.h>

#include <Serialization/ShapeSerialization.h>
#include <Serialization/DXSMSerialization.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class PlaneShapeObject_Info :
    public ShapeObject_Info<PlaneShapeData>
{
public:
	PlaneShapeObject_Info() {};

	explicit PlaneShapeObject_Info(SE::UUID uuid,
		SE_G::DeferredRenderer* renderSystem, PlaneShapeData initData = PlaneShapeData());
	explicit PlaneShapeObject_Info(
		SE_G::DeferredRenderer* renderSystem, PlaneShapeData initData = PlaneShapeData());

	static eastl::unique_ptr<PlaneShapeObject_Info> FromJson(
		SE_G::DeferredRenderer* renderSystem,
		const json& j);
};

