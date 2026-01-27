#pragma once

#include <Utils/StringUtils.h>
#include <EASTL/string.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class AssetPath
{
public:
	enum class AssetSource {
		Engine,
		Project,
		/* Only for devs! Don't show t0 users!*/ Absolute,
	} m_assetSource = AssetSource::Project;

	static eastl::wstring s_projectPath;

	explicit AssetPath(eastl::wstring relativePath = L"", AssetSource assetSource = AssetSource::Engine);

	eastl::wstring m_assetRelativePath;

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
