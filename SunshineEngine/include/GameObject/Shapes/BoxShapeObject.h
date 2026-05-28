#pragma once

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <GameObject/Shapes/ShapeObject.h>
#include <GameObject/Shapes/ShapeData.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class BoxShapeObject_Info :
	public ShapeObject_Info<BoxShapeData>
{
public:
	BoxShapeObject_Info() {};

	explicit BoxShapeObject_Info(SE::UUID uuid,
		SE_G::DeferredRenderer* renderSystem, BoxShapeData initData = BoxShapeData());
	explicit BoxShapeObject_Info(
		SE_G::DeferredRenderer* renderSystem, BoxShapeData initData = BoxShapeData());
	
	static eastl::unique_ptr<BoxShapeObject_Info> FromJson(
		SE_G::DeferredRenderer* renderSystem,
		const json& j);

	bool IsUvCubeMapMode();
	void SetUvCubeMapMode(SE_G::DeferredRenderer* renderSystem, bool cubeMapUV);
};
