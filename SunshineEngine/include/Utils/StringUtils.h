#pragma once
#include <EASTL/string.h>

inline eastl::wstring JoinWchar_Wstring(const wchar_t* a, const wchar_t* b) {
    eastl::wstring s;
    s.reserve(wcslen(a) + wcslen(b));
    s.append(a);
    s.append(b);
    return s;
}

inline wchar_t* JoinWchar_Wchar(const wchar_t* a, const wchar_t* b) {
    if (!a) a = L"";
    if (!b) b = L"";

    const size_t lenA = wcslen(a);
    const size_t lenB = wcslen(b);
    const size_t total = lenA + lenB + 1;

    wchar_t* out = static_cast<wchar_t*>(malloc(total * sizeof(wchar_t)));
    if (!out) return nullptr;

#if defined(_MSC_VER)
    // Copy and append with bounds checking on MSVC
    wcscpy_s(out, total, a);
    wcscat_s(out, total, b);
#else
    // Standard C functions (ensure buffer is large enough)
    wcscpy(out, a);
    wcscat(out, b);
#endif
    return out;
}

inline eastl::wstring MakeEngineAssetPath_Wstring(const wchar_t* sub) {
    return JoinWchar_Wstring(ENGINE_ASSETS_DIR, sub);
}

inline eastl::wstring MakeEngineAssetPath_Wchar(const wchar_t* sub) {
    return JoinWchar_Wchar(ENGINE_ASSETS_DIR, sub);
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

inline bool EASTLStringEquals(const eastl::string& a, const char* b) {
    return a.compare(b) == 0;
}
inline bool EASTLStringEquals(const eastl::string& a, const eastl::string& b) {
    return a == b;
}
