#pragma once
#include "./Utils/ILogManager.h"
#include <string>
#include <vector>

//      How to Log:
//      Call any needed function:
//
//      Game Log:
//          LOG_GAME_INFO(your string);
//          LOG_GAME_WARN(your string);
//          LOG_GAME_ERROR(your string);

class LogPanel;

class GameLogManager : public ILogManager
{
public:

    static GameLogManager& Get()
    {
        static GameLogManager instance;
        return instance;
    }

    void AddLog(LogTarget target, LogType type, const char* fmt, ...) override;

    static void AddInfoToGameLog(const char* fmt, ...);
    static void AddWarningToGameLog(const char* fmt, ...);
    static void AddErrorToGameLog(const char* fmt, ...);

    std::vector<LogEntry> GetLogs() const override;
    std::vector<LogEntry> GetLogs(LogTarget target) const override;

    void Clear() override;
    void Clear(LogTarget target) override;

private:
    GameLogManager() = default;
    std::vector<LogEntry> logs;
    void AddFormattedLog(LogTarget target, LogType type, const char* fmt, va_list args) override;
};

// Game 
#define LOG_GAME_INFO(fmt, ...)       GameLogManager::AddInfoToGameLog(fmt, ##__VA_ARGS__)
#define LOG_GAME_WARN(fmt, ...)       GameLogManager::AddWarningToGameLog(fmt, ##__VA_ARGS__)
#define LOG_GAME_ERROR(fmt, ...)      GameLogManager::AddErrorToGameLog(fmt, ##__VA_ARGS__)