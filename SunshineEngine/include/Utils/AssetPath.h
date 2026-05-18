#pragma once

#include <Utils/StringUtils.h>
#include <EASTL/string.h>

#include <Graphics/Bindable/Bindable.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class AssetPath
{
public:
	enum class AssetSource {
		Engine,
		Project,
		/* Only for devs! Don't show to users!*/ Absolute,
	} m_assetSource = AssetSource::Project;

	static eastl::wstring s_projectPath;
	eastl::wstring m_assetRelativePath;
	struct AdditionalParams {
		union {
			struct MeshParams
			{
				uint32_t param1 = 1; // slice, numSubdivisions
				uint32_t param2 = 1; // stack
			} asMesh;
			struct ShaderParams
			{
				SE_G::Bind::PipelineStage shaderType;
				UINT numInputElements;
				D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements;
			} asShader;
		};

	} m_params;

	explicit AssetPath(eastl::wstring relativePath = L"", AssetSource assetSource = AssetSource::Engine);

	eastl::wstring GetFullPath() const;

	json ToJson() const;
	void FromJson(const json& j);

	inline void to_json(json& j, const AssetPath& v) {
		j = v.ToJson();
	}
	inline void from_json(const json& j, AssetPath& v) {
		v.FromJson(j);
	}

	bool operator==(const AssetPath& other) const noexcept;
	bool operator!=(const AssetPath& other) const noexcept;

    // JSON enum mappings (serialize as readable strings)
	NLOHMANN_JSON_SERIALIZE_ENUM(AssetSource, {
		{AssetSource::Engine, "Engine"},
		{AssetSource::Project, "Project"},
		{AssetSource::Absolute, "Absolute"},
		});
};
