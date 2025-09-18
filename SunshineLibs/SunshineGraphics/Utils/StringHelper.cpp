

#include "StringHelper.h"
#include <iostream>

std::wstring StringHelper::StringToWide(std::string str)
{

    std::wstring wide_string(str.begin(), str.end());
    return wide_string;
}

std::string StringHelper::GetFileExtension(const std::string& filename)
{
	size_t off = filename.find_last_of('.');
	if (off == std::string::npos)
	{
		return {};
	}
	return std::string(filename.substr(off + 1));
}

std::string StringHelper::GetDirectoryFromPath(const std::string& filepath)
{
	size_t off1 = filepath.find_last_of('\\');
	size_t off2 = filepath.find_last_of('/');
	if (off1 == std::string::npos && off2 == std::string::npos) //If no slash or backslash in path?
	{
		return "";
	}
	if (off1 == std::string::npos)
	{
		return filepath.substr(0, off2);
	}
	if (off2 == std::string::npos)
	{
		return filepath.substr(0, off1);
	}
	//If both exists, need to use the greater offset
	return filepath.substr(0, std::max(off1, off2));
}

std::string StringHelper::GetFileNameWithoutExtension(const std::string& filepath)
{
    // Извлекаем имя файла с расширением из полного пути
    size_t slashOff1 = filepath.find_last_of('\\');
    size_t slashOff2 = filepath.find_last_of('/');
    size_t slashOff = (slashOff1 == std::string::npos) ?
        slashOff2 :
        ((slashOff2 == std::string::npos) ? slashOff1 : std::max(slashOff1, slashOff2));

    //std::cout << filepath << " :: " << slashOff1 << ", " << slashOff2 << ", " << std::string::npos << ", " << slashOff << "\n";
    //std::cout << filepath << " :: " << (slashOff1 == std::string::npos) << ", " << (slashOff2 == std::string::npos) << "\n";

    std::string filenameWithExtension;
    if (slashOff == std::string::npos)
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
    if (dotOff == std::string::npos)
    {
        // Если точки нет, возвращаем имя файла как есть
        return filenameWithExtension;
    }
    return filenameWithExtension.substr(0, dotOff);
}