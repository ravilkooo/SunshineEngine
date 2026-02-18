#pragma once
#include <string>
#include <vector>

#include "imgui.h"

class ILogManager
{
public:
    enum class LogType
    {
        Info,
        Warning,
        Error,
        Count
    };

    enum class LogTarget
    {
        Editor,
        Game
    };

    struct LogEntry
    {
        LogType type;
        std::string message;
        float color[4];
        LogTarget target;
    };

    struct LogTypeInfo
    {
        const char* name;
        float color[4];
    };

    inline static const LogTypeInfo s_LogTypeInfos[(int)LogType::Count] = {
        { "[Info] ",    {0.7f, 0.7f, 0.7f, 1.0f} },
        { "[Warning] ", {1.0f, 0.8f, 0.2f, 1.0f} },
        { "[Error] ",   {1.0f, 0.3f, 0.3f, 1.0f} }
    };

    virtual ~ILogManager() = default;

    virtual void AddLog(LogTarget target, LogType type, const char* fmt, ...) = 0;
    virtual std::vector<LogEntry> GetLogs() const = 0;
    virtual std::vector<LogEntry> GetLogs(LogTarget target) const = 0;
    virtual void Clear() = 0;
    virtual void Clear(LogTarget target) = 0;

protected:
    virtual void AddFormattedLog(LogTarget target, LogType type, const char* fmt, va_list args) = 0;
};
