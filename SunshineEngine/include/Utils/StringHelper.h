#pragma once

#include <EASTL/string.h>

class StringHelper
{
public:
	static eastl::wstring GetFileExtension(const eastl::wstring& filename);
	static eastl::wstring GetDirectoryFromPath(const eastl::wstring& filepath);
	static eastl::wstring GetFileNameWithoutExtension(const eastl::wstring& filename);
	static eastl::wstring StringToWide(const eastl::string& s);
};

