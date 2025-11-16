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
        ImVec4 color;
        LogTarget target;
    };

    virtual ~ILogManager() = default;

    virtual void AddLog(LogTarget target, LogType type, const char* fmt, ...) = 0;
    virtual std::vector<LogEntry> GetLogs(LogTarget target) const = 0;
    virtual void Clear(LogTarget target) = 0;

protected:
    virtual void AddFormattedLog(LogTarget target, LogType type, const char* fmt, va_list args) = 0;
};
