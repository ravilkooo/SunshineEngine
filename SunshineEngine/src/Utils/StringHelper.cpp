

#include "Utils/StringHelper.h"
#include <iostream>

eastl::wstring StringHelper::GetFileExtension(const eastl::wstring& filename)
{
	size_t off = filename.find_last_of('.');
	if (off == eastl::wstring::npos)
	{
		return {};
	}
	return eastl::wstring(filename.substr(off + 1));
}

eastl::wstring StringHelper::GetDirectoryFromPath(const eastl::wstring& filepath)
{
	size_t off1 = filepath.find_last_of('\\');
	size_t off2 = filepath.find_last_of('/');
	if (off1 == eastl::wstring::npos && off2 == eastl::wstring::npos) //If no slash or backslash in path?
	{
		return L"";
	}
	if (off1 == eastl::wstring::npos)
	{
		return filepath.substr(0, off2);
	}
	if (off2 == eastl::wstring::npos)
	{
		return filepath.substr(0, off1);
	}
	//If both exists, need to use the greater offset
	return filepath.substr(0, std::max(off1, off2));
}

eastl::wstring StringHelper::GetFileNameWithoutExtension(const eastl::wstring& filepath)
{
    // Извлекаем имя файла с расширением из полного пути
    size_t slashOff1 = filepath.find_last_of('\\');
    size_t slashOff2 = filepath.find_last_of('/');
    size_t slashOff = (slashOff1 == eastl::wstring::npos) ?
        slashOff2 :
        ((slashOff2 == eastl::wstring::npos) ? slashOff1 : std::max(slashOff1, slashOff2));

    //std::cout << filepath << " :: " << slashOff1 << ", " << slashOff2 << ", " << eastl::wstring::npos << ", " << slashOff << "\n";
    //std::cout << filepath << " :: " << (slashOff1 == eastl::wstring::npos) << ", " << (slashOff2 == eastl::wstring::npos) << "\n";

    eastl::wstring filenameWithExtension;
    if (slashOff == eastl::wstring::npos)
    {
        // Если слэшей нет, то это уже имя файла
        filenameWithExtension = filepath;
    }
    else
    {
        // Иначе извлекаем подстроку после последнего слэша
        filenameWithExtension = filepath.substr(slashOff + 1);
    }

    // Удаляем расширение
    size_t dotOff = filenameWithExtension.find_last_of('.');
    if (dotOff == eastl::wstring::npos)
    {
        // Если точки нет, возвращаем имя файла как есть
        return filenameWithExtension;
    }
    return filenameWithExtension.substr(0, dotOff);
}