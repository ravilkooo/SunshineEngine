#pragma once

#include <EASTL/string.h>
#include <string>
#include <windows.h>

inline eastl::string WcharToChar(const wchar_t* wstr) {
    if (!wstr) return eastl::string();

    int size_needed = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr,
        -1,
        NULL,
        0,
        NULL, NULL
    );
    if (size_needed == 0) return eastl::string();

    // Use a temporary std::string as a writable buffer, then construct eastl::string
    std::string tmp;
    tmp.resize(size_needed);
    int converted_chars = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr,
        -1,
        tmp.data(),
        size_needed,
        NULL, NULL
    );

    if (converted_chars == 0) return eastl::string();

    // converted_chars includes the null terminator; omit it in the returned string
    return eastl::string(tmp.c_str(), static_cast<size_t>(converted_chars - 1));
}

inline eastl::wstring JoinWchar_Wstring(const wchar_t* a, const wchar_t* b) {
    if (!a) a = L"";
    if (!b) b = L"";
    eastl::wstring s;
    s.reserve(wcslen(a) + wcslen(b));
    s.append(a);
    s.append(b);
    return s;
}

inline eastl::string JoinChar_Char(const char* a, const char* b) {
    if (!a) a = "";
    if (!b) b = "";
    eastl::string s;
    s.reserve(strlen(a) + strlen(b));
    s.append(a);
    s.append(b);
    return s;
}

inline eastl::wstring MakeEngineAssetPath_Wstring(const wchar_t* sub) {
    return JoinWchar_Wstring(ENGINE_ASSETS_DIR, sub);
}


inline eastl::string MakeEngineAssetPath_Char(const char* sub) {
    // Convert ENGINE_ASSETS_DIR to UTF-8 string and join
    eastl::string dirUtf8 = WcharToChar(ENGINE_ASSETS_DIR);
    return JoinChar_Char(dirUtf8.c_str(), sub);
}

inline eastl::string wstringToString(const eastl::wstring& wideStr) {
    eastl::string result;
    result.reserve(wideStr.size());
    for (auto wc : wideStr)
        result.push_back(static_cast<char>(wc));
    return result;
}

inline eastl::string to_string_eastl(int value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return eastl::string(buffer);
}

inline bool EASTLStringEqualsChar(const eastl::string& a, const char* b) {
    return a.compare(b) == 0;
}

inline bool EASTLStringEqualsString(const eastl::string& a, const eastl::string& b) {
    return a == b;
}

inline std::string EASTLToStdString(const eastl::string& s) {
    return std::string(s.data(), s.size());
}

inline eastl::string StdToEASTLString(const std::string& s) {
    return eastl::string(s.data(), s.size());
}