#pragma once

#include <EASTL/string.h>

#include <Utils/StringUtils.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;


// Converter for eastl::wstring to/from JSON using UTF-8 strings
/*
static void to_json(json& j, const eastl::wstring& wstr) {
    j = wstringToString(wstr).c_str();
}

static void from_json(const json& j, eastl::wstring& wstr) {
    wstr = ToEastlWstring(j.get<std::string>().c_str());
}
*/
