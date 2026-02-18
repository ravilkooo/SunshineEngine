#pragma once
#include "./Utils/ILogManager.h"
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

class LogPanel;

class EditorLogManager : public ILogManager
{
public:
    static EditorLogManager& Get()
    {
        static EditorLogManager instance;
        return instance;
    }
    
    void AddLog(LogTarget target, LogType type, const char* fmt, ...) IM_FMTARGS(3) override;
    
    static void AddInfoToEditorLog(const char* fmt, ...) IM_FMTARGS(1);
    static void AddWarningToEditorLog(const char* fmt, ...) IM_FMTARGS(1);
    static void AddErrorToEditorLog(const char* fmt, ...) IM_FMTARGS(1);

    /*
    static void AddInfoToGameLog(const char* fmt, ...) IM_FMTARGS(1);
    static void AddWarningToGameLog(const char* fmt, ...) IM_FMTARGS(1);
    static void AddErrorToGameLog(const char* fmt, ...) IM_FMTARGS(1);
    */
    
    std::vector<LogEntry> GetLogs() const override;
    std::vector<LogEntry> GetLogs(LogTarget target) const override;
    
    void Clear() override;
    void Clear(LogTarget target) override;

private:
    EditorLogManager() = default;
    std::vector<LogEntry> logs;
    void AddFormattedLog(LogTarget target, LogType type, const char* fmt, va_list args) override;
};

// Editor 
#define LOG_EDITOR_INFO(fmt, ...)     EditorLogManager::AddInfoToEditorLog(fmt, ##__VA_ARGS__)
#define LOG_EDITOR_WARN(fmt, ...)     EditorLogManager::AddWarningToEditorLog(fmt, ##__VA_ARGS__)
#define LOG_EDITOR_ERROR(fmt, ...)    EditorLogManager::AddErrorToEditorLog(fmt, ##__VA_ARGS__)
