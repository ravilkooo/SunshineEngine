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
class ShapeObject_Info : public GameObject_Info
{
public:
	eastl::shared_ptr<T> m_shapeData;

	json ToJson() const override  {
		json j = GameObject_Info::ToJson();
		j["m_shapeData"] = *(m_shapeData.get());
		return j;
	}
};
