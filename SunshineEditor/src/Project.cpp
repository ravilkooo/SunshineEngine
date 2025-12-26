#include <filesystem>
#include <Project.h>
#include <Utils/StringUtils.h>
#include <WorldEditor.h>
#include <Utils/AssetPath.h>

#include "EASTL/shared_ptr.h"

namespace SE
{
	WorldEditor* Project::s_worldEditor = nullptr;
	
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

	eastl::string Project::CreateNew(const eastl::string& name,
                           Project& outProject)
    {
        if (name.empty())
            return "Project name cannot be empty";

        eastl::wstring wName = Utf8ToWString(name);
        eastl::wstring projectPath = wName + L"/";
		
        ProjectList projects;
	    
	    eastl::string error = LoadProjects(projects);
	    if (!error.empty())
	        return "Failed to load project list: " + error;
		
        if (isDuplicateName(wName, projects))
            return "Project with name '" + name + "' already exists";
		
        Project newProject(projectPath);
        newProject.SetCreationDate(std::chrono::system_clock::now());
        newProject.SetLastSavedTime(std::chrono::system_clock::now());
		
	    error = newProject.createDirectory();
	    if (!error.empty())
	        return "Failed to create project directory: " + error;
		
		error = createInitialScene(newProject);
		if (!error.empty())
		{
			newProject.deleteDirectory();
			return"Failed to create project scene: " + error;
		}
		
        projects.push_back(newProject);
	    error = SaveProjects(projects);
	    if (!error.empty())
	    {
	        newProject.deleteDirectory();
			projects.pop_back();
	        return "Failed to save project list: " + error;
	    }
		
        outProject = projects.back();
		
        return "";
    }

	eastl::string Project::Open() const
	{
		if (!s_worldEditor)
			return "WorldEditor is null";
    
		eastl::wstring scenePath = GetScenePath();

		//GetFullPath();
		AssetPath::s_projectPath = GetFullPath();
		
		if (!std::filesystem::exists(scenePath.c_str()))
		{
			return "Scene file not found: " + WStringToUtf8(scenePath);
		}
		
		if (!s_worldEditor->LoadScene(scenePath.c_str()))
		{
			return "Failed to load scene from: " + WStringToUtf8(scenePath);
		}
    
		return "";
	}

	eastl::string Project::Save()
	{
		if (!s_worldEditor)
			return "WorldEditor is null";
    
		eastl::wstring scenePath = GetScenePath();
		
		s_worldEditor->SaveScene(scenePath.c_str());
		
		UpdateSaveTime();

		ProjectList projects;
		eastl::string error = LoadProjects(projects);
		if (!error.empty())
			return "Failed to load project list: " + error;
		for (auto& project : projects)
		{
			if (project.GetSubPath() == GetSubPath())
			{
				project.UpdateSaveTime();
				break;
			}
		}
		error = SaveProjects(projects);
		if (!error.empty())
		{
			return "Failed to save updated project list: " + error;
		}
		return "";
    }

    eastl::string Project::Rename(const eastl::string& newName)
    {
        if (newName.empty())
        	return "New project name cannot be empty";

        eastl::wstring wNewName = Utf8ToWString(newName);
        eastl::wstring newPath = wNewName + L"/";
		ProjectList projects;

		eastl::string error = LoadProjects(projects);
		if (!error.empty())
			return "Failed to load project list: " + error;
		
        int currentIndex = -1;
        for (size_t i = 0; i < projects.size(); ++i)
        {
            if (projects[i].GetSubPath() == m_subPath)
            {
                currentIndex = static_cast<int>(i);
                break;
            }
        }

        if (currentIndex == -1)
        	return "Current project not found in list: " + WStringToUtf8(m_subPath);
		
		if (isDuplicateName(wNewName, projects, currentIndex))
			return "Project with name '" + newName + "' already exists";
		
		error = renameDirectory(newPath);
		if (!error.empty())
			return "Failed to rename project directory: " + error;

        eastl::wstring oldPath = m_subPath;
        m_subPath = newPath;
		
        for (auto& project : projects)
        {
            if (project.GetSubPath() == oldPath)
            {
                project.SetSubPath(newPath);
                break;
            }
        }
		
		error = SaveProjects(projects);
		if (!error.empty())
		{
			m_subPath = oldPath;
			renameDirectory(oldPath);
			return "Failed to save updated project list: " + error;
		}
        return "";
    }

	eastl::string Project::Delete(bool deleteFilesFromDisk)
	{
		if (deleteFilesFromDisk)
		{
			eastl::string error = deleteDirectory();
			if (!error.empty())
			{
				return "Failed to delete project files: " + error;
			}
		}
		
		eastl::vector<Project> projects;
		eastl::string error = LoadProjects(projects);
		if (!error.empty())
			return "Failed to load project list: " + error;
		
		size_t originalSize = projects.size();
		projects.erase(
			eastl::remove_if(projects.begin(), projects.end(),
				[this](const Project& p) {
					return p.GetSubPath() == m_subPath;
				}),
			projects.end()
		);
		
		if (projects.size() == originalSize)
		{
			return ""; 
		}
		
		error = SaveProjects(projects);
		if (!error.empty())
			return "Failed to save project list: " + error;

		return "";
	}

	eastl::string Project::createDirectory() const
    {
        try
        {
            eastl::wstring fullPath = GetFullPath();
            std::filesystem::create_directories(fullPath.c_str());
        	if (std::filesystem::exists(fullPath.c_str()))
        		return "";
        	else
        		return "Failed to create directory: " + WStringToUtf8(fullPath);
        }
        catch (const std::exception& e)
        {
        	return "Failed to create directory: " + eastl::string(e.what());
        }
    }

    eastl::string Project::renameDirectory(const eastl::wstring& newPath) const
    {
        try
        {
            eastl::wstring oldFullPath = GetFullPath();
            eastl::wstring newFullPath = JoinWchar_Wstring(PROJECTS_DIR, newPath.c_str());
            
            if (std::filesystem::exists(newFullPath.c_str()))
                return "Failed to rename directory: " + WStringToUtf8(newFullPath);
            
            std::filesystem::rename(oldFullPath.c_str(), newFullPath.c_str());
            return "";
        }
        catch (const std::exception& e)
        {
            return "Failed to rename directory: " + eastl::string(e.what());;
        }
    }

    eastl::string Project::deleteDirectory() const
    {
        try
        {
            eastl::wstring fullPath = GetFullPath();
            std::filesystem::remove_all(fullPath.c_str());
            return "";
        }
        catch (const std::exception& e)
        {
            return "Failed to delete directory: " + eastl::string(e.what());;
        }
    }

	eastl::string Project::createInitialScene(const Project& project)
	{
	    try
	    {
	    	eastl::wstring subPath = project.GetSubPath();
	    	if (subPath.back() != L'/' && subPath.back() != L'\\')
	    		subPath.push_back(L'/');
	    	
	    	eastl::wstring sceneFile = JoinWchar_Wstring(project.GetFullPath().c_str(), L"scene.json");
	    	eastl::wstring templateFile = JoinWchar_Wstring(PROJECTS_DIR, L"Templates/DefaultScene.json");
	    	std::filesystem::copy(templateFile.c_str(), sceneFile.c_str());
	    	//std::rename("from.txt", "to.txt")
    		
	        return "";
	    }
	    catch (const std::exception& e)
	    {
	        return "Failed to create scene: " + eastl::string(e.what());
	    }
	}

    bool Project::Exists(const eastl::wstring& path)
    {
        eastl::wstring fullPath = JoinWchar_Wstring(PROJECTS_DIR, path.c_str());
        return std::filesystem::exists(fullPath.c_str());
    }

    bool Project::ExistsOnDisk() const
    {
        return Exists(m_subPath);
    }

    bool isDuplicateName(const eastl::wstring& name,
		const ProjectList& projects,
		int excludeIndex)
    {
        for (size_t i = 0; i < projects.size(); ++i)
        {
            if (excludeIndex >= 0 && static_cast<int>(i) == excludeIndex)
                continue;

            eastl::wstring existingPath = projects[i].GetSubPath();
            eastl::wstring existingName = existingPath;
            
            if (!existingName.empty() && existingName.back() == L'/')
                existingName.pop_back();
            
            size_t slashPos = existingName.find_last_of(L'/');
            if (slashPos != eastl::wstring::npos)
                existingName = existingName.substr(slashPos + 1);
            
            if (existingName == name)
                return true;
        }
        
        return false;
    }

	eastl::wstring Project::GetSubPath() const
	{
		return m_subPath;
	}

	eastl::wstring Project::GetFullPath() const
	{
		return JoinWchar_Wstring(PROJECTS_DIR, m_subPath.c_str());
	}

	eastl::wstring Project::GetScenePath() const
	{
		return JoinWchar_Wstring(GetFullPath().c_str(), L"scene.json");
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
