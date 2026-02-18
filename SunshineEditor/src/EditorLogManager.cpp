#include "EditorLogManager.h"

void EditorLogManager::AddLog(LogTarget target, LogType type, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    AddFormattedLog(target, type, fmt, args);
    va_end(args);
}

std::vector<EditorLogManager::LogEntry> EditorLogManager::GetLogs() const
{
    return logs;
}

std::vector<EditorLogManager::LogEntry> EditorLogManager::GetLogs(LogTarget target) const
{
    std::vector<LogEntry> filtered;
    for (auto& log : logs)
        if (log.target == target)
            filtered.push_back(log);
    return filtered;
}

void EditorLogManager::Clear()
{
    logs.clear();
}

void EditorLogManager::Clear(LogTarget target)
{
    logs.erase(
        std::remove_if(logs.begin(), logs.end(),
            [target](const LogEntry& log) { return log.target == target; }),
        logs.end());
}

void EditorLogManager::AddFormattedLog(LogTarget target, LogType type, const char* fmt, va_list args)
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

void EditorLogManager::AddInfoToEditorLog(const char* fmt, ...)
{
    va_list args; va_start(args, fmt);
    Get().AddFormattedLog(LogTarget::Editor, LogType::Info, fmt, args);
    va_end(args);
}

void EditorLogManager::AddWarningToEditorLog(const char* fmt, ...)
{
    va_list args; va_start(args, fmt);
    Get().AddFormattedLog(LogTarget::Editor, LogType::Warning, fmt, args);
    va_end(args);
}

void EditorLogManager::AddErrorToEditorLog(const char* fmt, ...)
{
    va_list args; va_start(args, fmt);
    Get().AddFormattedLog(LogTarget::Editor, LogType::Error, fmt, args);
    va_end(args);
}
