#pragma once
#include <imgui.h>
#include <string>
#include <vector>

class LogPanel;

class LogManager
{
public:
    enum class LogType
    {
        Info,
        Warning,
        Error,
        Count
    };

    struct LogTypeInfo
    {
        const char* name;
        ImVec4 color;
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

    inline static const LogTypeInfo s_LogTypeInfos[(int)LogType::Count] = {
        { "[Info] ",    ImVec4(0.7f, 0.7f, 0.7f, 1.0f) },
        { "[Warning] ", ImVec4(1.0f, 0.8f, 0.2f, 1.0f) },
        { "[Error] ",   ImVec4(1.0f, 0.3f, 0.3f, 1.0f) }
    };

    static LogManager& Get()
    {
        static LogManager instance;
        return instance;
    }
    
    void AddLog(LogTarget target, LogType type, const char* fmt, ...) IM_FMTARGS(3);
    
    std::vector<LogEntry> GetLogs(LogTarget target) const;
    
    void Clear(LogTarget target);

private:
    LogManager() = default;
    std::vector<LogEntry> logs;
};
