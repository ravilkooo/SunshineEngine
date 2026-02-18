#include <Utils/GameLogManager.h>

void GameLogManager::AddLog(LogTarget target, LogType type, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    AddFormattedLog(target, type, fmt, args);
    va_end(args);
}

std::vector<GameLogManager::LogEntry> GameLogManager::GetLogs() const
{
    return logs;
}

std::vector<GameLogManager::LogEntry> GameLogManager::GetLogs(LogTarget target) const
{
    std::vector<LogEntry> filtered;
    for (auto& log : logs)
        if (log.target == target)
            filtered.push_back(log);
    return filtered;
}

void GameLogManager::Clear()
{
    logs.clear();
}

void GameLogManager::Clear(LogTarget target)
{
    logs.erase(
        std::remove_if(logs.begin(), logs.end(),
            [target](const LogEntry& log) { return log.target == target; }),
        logs.end());
}

void GameLogManager::AddFormattedLog(LogTarget target, LogType type, const char* fmt, va_list args)
{
    const LogTypeInfo& info = s_LogTypeInfos[(int)type];

    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);

    std::string finalMsg = info.name;
    finalMsg += buffer;

    LogEntry entry;
    entry.type = type;
    entry.message = finalMsg;
    entry.target = target;
    // entry.color = info.color;
    memcpy(entry.color, info.color, sizeof(float) * 4);

    logs.push_back(entry);
}

void GameLogManager::AddInfoToGameLog(const char* fmt, ...)
{
    va_list args; va_start(args, fmt);
    Get().AddFormattedLog(LogTarget::Game, LogType::Info, fmt, args);
    va_end(args);
}

void GameLogManager::AddWarningToGameLog(const char* fmt, ...)
{
    va_list args; va_start(args, fmt);
    Get().AddFormattedLog(LogTarget::Game, LogType::Warning, fmt, args);
    va_end(args);
}

void GameLogManager::AddErrorToGameLog(const char* fmt, ...)
{
    va_list args; va_start(args, fmt);
    Get().AddFormattedLog(LogTarget::Game, LogType::Error, fmt, args);
    va_end(args);
}
