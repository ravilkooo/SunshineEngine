#pragma once
#include <imgui.h>
#include <string>
#include <vector>

//      How to Log:
//      Call any needed function:
//
//      Editor Log:
//          LOG_EDITOR_INFO(your string);     
//          LOG_EDITOR_WARN(your string);
//          LOG_EDITOR_ERROR(your string);  
//
//      Game Log:
//          LOG_GAME_INFO(your string);
//          LOG_GAME_WARN(your string);
//          LOG_GAME_ERROR(your string);

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
    
    static void AddInfoToEditorLog(const char* fmt, ...) IM_FMTARGS(1);
    static void AddWarningToEditorLog(const char* fmt, ...) IM_FMTARGS(1);
    static void AddErrorToEditorLog(const char* fmt, ...) IM_FMTARGS(1);

    static void AddInfoToGameLog(const char* fmt, ...) IM_FMTARGS(1);
    static void AddWarningToGameLog(const char* fmt, ...) IM_FMTARGS(1);
    static void AddErrorToGameLog(const char* fmt, ...) IM_FMTARGS(1);
    
    std::vector<LogEntry> GetLogs(LogTarget target) const;
    
    void Clear(LogTarget target);

private:
    LogManager() = default;
    std::vector<LogEntry> logs;
    void AddFormattedLog(LogTarget target, LogType type, const char* fmt, va_list args);
};

// Editor 
#define LOG_EDITOR_INFO(fmt, ...)     LogManager::AddInfoToEditorLog(fmt, ##__VA_ARGS__)
#define LOG_EDITOR_WARN(fmt, ...)     LogManager::AddWarningToEditorLog(fmt, ##__VA_ARGS__)
#define LOG_EDITOR_ERROR(fmt, ...)    LogManager::AddErrorToEditorLog(fmt, ##__VA_ARGS__)

// Game 
#define LOG_GAME_INFO(fmt, ...)       LogManager::AddInfoToGameLog(fmt, ##__VA_ARGS__)
#define LOG_GAME_WARN(fmt, ...)       LogManager::AddWarningToGameLog(fmt, ##__VA_ARGS__)
#define LOG_GAME_ERROR(fmt, ...)      LogManager::AddErrorToGameLog(fmt, ##__VA_ARGS__)