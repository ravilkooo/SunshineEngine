#pragma once

#include <EASTL/string.h>
#include <string>
#include <windows.h>

inline char* WcharToChar(const wchar_t* wstr) {
    if (!wstr) return nullptr;

    // Calculate required buffer size (in bytes) for UTF-8 encoding
    int size_needed = WideCharToMultiByte(
        CP_UTF8,               // Convert to UTF-8
        0,                     // No special flags
        wstr,                  // Source wchar_t string
        -1,                    // Null-terminated input string
        NULL,                  // No output buffer yet
        0,                     // Request buffer size
        NULL, NULL             // No default char or used flag
    );
    if (size_needed == 0) return nullptr;

    // Allocate buffer for converted string
    char* buffer = static_cast<char*>(malloc(size_needed));
    if (!buffer) return nullptr;

    // Perform the conversion
    int converted_chars = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr,
        -1,
        buffer,
        size_needed,
        NULL, NULL
    );

    if (converted_chars == 0) {
        free(buffer);
        return nullptr;
    }

    return buffer;
}

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

inline char* JoinChar_Char(const char* a, const char* b) {
    if (!a) a = "";
    if (!b) b = "";

    const size_t lenA = strlen(a);
    const size_t lenB = strlen(b);
    const size_t total = lenA + lenB + 1; // +1 for null terminator

    char* out = static_cast<char*>(malloc(total * sizeof(char)));
    if (!out) return nullptr;

#if defined(_MSC_VER)
    strcpy_s(out, total, a);
    strcat_s(out, total, b);
#else
    strcpy(out, a);
    strcat(out, b);
#endif

    return out;
}

inline eastl::wstring MakeEngineAssetPath_Wstring(const wchar_t* sub) {
    return JoinWchar_Wstring(ENGINE_ASSETS_DIR, sub);
}

inline wchar_t* MakeEngineAssetPath_Wchar(const wchar_t* sub) {
    return JoinWchar_Wchar(ENGINE_ASSETS_DIR, sub);
}

inline char* MakeEngineAssetPath_Char(const char* sub) {
    return JoinChar_Char(WcharToChar(ENGINE_ASSETS_DIR), sub);
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
