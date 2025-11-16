#pragma once

#include <Graphics/Bindable/Sampler.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

NLOHMANN_JSON_SERIALIZE_ENUM(SE_G::Bind::SamplerPreset, {
	{SE_G::Bind::SamplerPreset::Wrap, "Wrap"},
	{SE_G::Bind::SamplerPreset::Mirror, "Mirror"},
	{SE_G::Bind::SamplerPreset::Clamp, "Clamp"},
	{SE_G::Bind::SamplerPreset::Border, "Border"},
	})


NLOHMANN_JSON_SERIALIZE_ENUM(SE_G::Bind::PipelineStage, {
	{SE_G::Bind::PipelineStage::VERTEX_SHADER, "VERTEX_SHADER"},
	{SE_G::Bind::PipelineStage::PIXEL_SHADER, "PIXEL_SHADER"},
	{SE_G::Bind::PipelineStage::COMPUTE_SHADER, "COMPUTE_SHADER"},
	})

