#include "LogManager.h"

void LogManager::AddLog(LogTarget target, LogType type, const char* fmt, ...)
{
    const LogTypeInfo& info = s_LogTypeInfos[(int)type];
    
    std::string finalMsg = info.name;

    va_list args;
    va_start(args, fmt);

    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    finalMsg += buffer;

    va_end(args);

    logs.push_back({ type, finalMsg, info.color, target });
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
