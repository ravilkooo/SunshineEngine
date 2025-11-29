#include "LogManager.h"

void LogManager::AddLog(LogTarget target, LogType type, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    AddFormattedLog(target, type, fmt, args);
    va_end(args);
}

std::vector<LogManager::LogEntry> LogManager::GetLogs(LogTarget target) const
{
    std::vector<LogEntry> filtered;
    for (auto& log : logs)
        if (log.target == target)
            filtered.push_back(log);
    return filtered;
}

void LogManager::Clear(LogTarget target)
{
    logs.erase(
        std::remove_if(logs.begin(), logs.end(),
            [target](const LogEntry& log) { return log.target == target; }),
        logs.end());
}

void LogManager::AddFormattedLog(LogTarget target, LogType type, const char* fmt, va_list args)
{
    const LogTypeInfo& info = s_LogTypeInfos[(int)type];

    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);

    std::string finalMsg = info.name;
    finalMsg += buffer;
    ImVec4 color = info.color;
    
    LogEntry entry;
    entry.type = type;
    entry.message = finalMsg;
    entry.target = target;
    entry.color = color;
    
    logs.push_back(entry);
}

#define IMPLEMENT_LOG_METHODS(target_name, target_enum)                          \
void LogManager::AddInfoTo##target_name##Log(const char* fmt, ...)               \
{                                                                                \
    va_list args; va_start(args, fmt);                                           \
    Get().AddFormattedLog(target_enum, LogType::Info, fmt, args);                \
    va_end(args);                                                                \
}                                                                                \
void LogManager::AddWarningTo##target_name##Log(const char* fmt, ...)            \
{                                                                                \
    va_list args; va_start(args, fmt);                                           \
    Get().AddFormattedLog(target_enum, LogType::Warning, fmt, args);             \
    va_end(args);                                                                \
}                                                                                \
void LogManager::AddErrorTo##target_name##Log(const char* fmt, ...)              \
{                                                                                \
    va_list args; va_start(args, fmt);                                           \
    Get().AddFormattedLog(target_enum, LogType::Error, fmt, args);               \
    va_end(args);                                                                \
}

IMPLEMENT_LOG_METHODS(Editor, LogTarget::Editor)
IMPLEMENT_LOG_METHODS(Game, LogTarget::Game)

#undef IMPLEMENT_LOG_METHODS