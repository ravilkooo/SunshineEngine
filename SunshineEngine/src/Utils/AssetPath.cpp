#include <Utils/AssetPath.h>

AssetPath::AssetPath(eastl::wstring relativePath, AssetSource assetSource)
	: m_assetRelativePath(relativePath), m_assetSource(assetSource)
{

}

eastl::wstring AssetPath::s_projectPath = eastl::wstring(L"");

eastl::wstring AssetPath::GetFullPath()
{
	if (m_assetSource == AssetSource::Project)
	{
		return s_projectPath + m_assetRelativePath;
	}
	else
	{
		return ENGINE_ASSETS_DIR + m_assetRelativePath;
	}
}

json AssetPath::ToJson() const
{
	json j;
	j = nlohmann::json{
		{"relativePath", WStringToUtf8(m_assetRelativePath).c_str()},
		{"assetSource", m_assetSource}
	};
	return j;
}

void AssetPath::FromJson(const json& j)
{
	if (j.contains("relativePath"))
	{
		m_assetRelativePath = Utf8ToWString(j["relativePath"].get<std::string>().c_str());
	}
	if (j.contains("assetSource"))
		j.at("assetSource").get_to(m_assetSource);
}
