#include <Project.h>
#include <Utils/StringUtils.h>

namespace SE
{
	Project::Project()
		: m_subPath(L"DefaultScene/")
		, m_lastSavedTime(std::chrono::system_clock::now())
		, m_creationDate(std::chrono::system_clock::now())
	{
	}

	Project::Project(const eastl::wstring& subPath)
		: m_subPath(subPath)
		, m_lastSavedTime(std::chrono::system_clock::now())
		, m_creationDate(std::chrono::system_clock::now())
	{
	}

	eastl::wstring Project::GetSubPath() const
	{
		return m_subPath;
	}

	eastl::wstring Project::GetFullPath() const
	{
		return JoinWchar_Wstring(PROJECTS_DIR, m_subPath.c_str());
	}

	std::chrono::system_clock::time_point Project::GetLastSavedTime() const
	{
		return m_lastSavedTime;
	}

	std::chrono::system_clock::time_point Project::GetCreationDate() const
	{
		return m_creationDate;
	}

	void Project::SetSubPath(const eastl::wstring& subPath)
	{
		m_subPath = subPath;
	}

	void Project::SetLastSavedTime(const std::chrono::system_clock::time_point& lastSaved)
	{
		m_lastSavedTime = lastSaved;
	}

	void Project::SetCreationDate(const std::chrono::system_clock::time_point& creationDate)
	{
		m_creationDate = creationDate;
	}

	void Project::UpdateSaveTime()
	{
		m_lastSavedTime = std::chrono::system_clock::now();
	}
}
