#pragma once
#include <chrono>

inline int64_t ToUnixMillis(std::chrono::system_clock::time_point tp)
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(tp.time_since_epoch()).count();
}

inline std::chrono::system_clock::time_point FromUnixMillis(int64_t ms)
{
    using namespace std::chrono;
    return std::chrono::system_clock::time_point(milliseconds(ms));
}

inline std::wstring FormatTime(const std::chrono::system_clock::time_point& tp)
{
    using namespace std::chrono;

    std::time_t t = system_clock::to_time_t(tp); // truncate to seconds [web:44][web:46][web:52]
    std::tm tmLocal{};
#if defined(_WIN32)
    localtime_s(&tmLocal, &t);
#else
    localtime_r(&t, &tmLocal);
#endif

    wchar_t buffer[64];
    std::wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%Y-%m-%d %H:%M:%S", &tmLocal);
    return buffer;
}