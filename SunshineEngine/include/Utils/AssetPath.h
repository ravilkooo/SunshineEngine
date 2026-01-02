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

    // JSON enum mappings (serialize as readable strings)
	NLOHMANN_JSON_SERIALIZE_ENUM(AssetSource, {
		{AssetSource::Engine, "Engine"},
		{AssetSource::Project, "Project"},
		{AssetSource::Absolute, "Absolute"},
		});
};
