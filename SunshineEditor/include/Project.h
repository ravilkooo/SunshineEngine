#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <EASTL/string.h>

#include <Utils/StringUtils.h>
#include <Utils/TimeUtils.h>

namespace SE
{
	class Project
	{
	public:
        Project();
        Project(const eastl::wstring& subPath);

		eastl::wstring GetSubPath() const;
		eastl::wstring GetFullPath() const;

		std::chrono::system_clock::time_point GetLastSavedTime() const;
		std::chrono::system_clock::time_point GetCreationDate() const;

        void SetSubPath(const eastl::wstring& subPath);
        void SetLastSavedTime(const std::chrono::system_clock::time_point& lastSaved);
        void SetCreationDate(const std::chrono::system_clock::time_point& creationDate);

		void UpdateSaveTime();

	private:
		// FullPath = PROJECTS_DIR + m_subPath
		eastl::wstring m_subPath;
		std::chrono::system_clock::time_point m_lastSavedTime;
		std::chrono::system_clock::time_point m_creationDate;
	};

    using ProjectList = eastl::vector<Project>;

    inline bool SaveProjects(const ProjectList& projects)
    {
        eastl::wstring filePath = JoinWchar_Wstring(PROJECTS_DIR, L"projlist").c_str();

        std::ofstream out(filePath.c_str(), std::ios::binary);
        if (!out)
            return false;

        const uint32_t magic = 0x50524F4A; // 'JROP'
        const uint32_t version = 1;
        const uint32_t count = static_cast<uint32_t>(projects.size());

        out.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
        out.write(reinterpret_cast<const char*>(&version), sizeof(version));
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));

        if (!out)
            return false;

        for (const Project& p : projects)
        {
            eastl::string subPathUtf8 = WStringToUtf8(p.GetSubPath());

            uint32_t len = static_cast<uint32_t>(subPathUtf8.size());
            int64_t lastSaved = ToUnixMillis(p.GetLastSavedTime());
            int64_t created = ToUnixMillis(p.GetCreationDate());

            out.write(reinterpret_cast<const char*>(&len), sizeof(len));
            out.write(subPathUtf8.data(), len);
            out.write(reinterpret_cast<const char*>(&lastSaved), sizeof(lastSaved));
            out.write(reinterpret_cast<const char*>(&created), sizeof(created));

            if (!out)
                return false;
        }

        return true;
    }

    inline bool LoadProjects(ProjectList& projects)
    {

        eastl::wstring filePath = JoinWchar_Wstring(PROJECTS_DIR, L"projlist").c_str();
        //std::wcout << filePath << L"\n";

        std::ifstream in(filePath.c_str(), std::ios::binary);
        if (!in)
            return false;

        uint32_t magic = 0, version = 0, count = 0;
        in.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        in.read(reinterpret_cast<char*>(&version), sizeof(version));
        in.read(reinterpret_cast<char*>(&count), sizeof(count));

        if (!in || magic != 0x50524F4A || version != 1)
            return false;

        projects.clear();
        projects.reserve(count);

        for (uint32_t i = 0; i < count; ++i)
        {
            uint32_t len = 0;
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            if (!in)
                return false;

            eastl::string subPathUtf8;
            subPathUtf8.resize(len);
            in.read(&subPathUtf8[0], len);

            int64_t lastSaved = 0;
            int64_t created = 0;
            in.read(reinterpret_cast<char*>(&lastSaved), sizeof(lastSaved));
            in.read(reinterpret_cast<char*>(&created), sizeof(created));
            if (!in)
                return false;

            Project p(Utf8ToWString(subPathUtf8));
            p.SetSubPath(Utf8ToWString(subPathUtf8));
            p.SetCreationDate(FromUnixMillis(created));
            p.SetLastSavedTime(FromUnixMillis(lastSaved));

            projects.push_back(eastl::move(p));
        }

        return true;
    }

    inline void PrintProjectsToConsole(const ProjectList& projects)
    {
        std::wcout << L"Projects (" << projects.size() << L"):\n";

        for (size_t i = 0; i < projects.size(); ++i)
        {
            const Project& p = projects[i];

            std::wcout << L"[" << i << L"]\n";
            std::wcout << L"  SubPath     : " << p.GetSubPath().c_str() << L"\n";
            //std::wcout << L"  FullPath    : " << p.GetFullPath().c_str() << L"\n";
            std::wcout << L"  Created     : " << FormatTime(p.GetCreationDate()) << L"\n";
            std::wcout << L"  Last Saved  : " << FormatTime(p.GetLastSavedTime()) << L"\n";
            std::wcout << L"\n";
        }
    }
}

