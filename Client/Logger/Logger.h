#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

class Logger {
public:
    enum class Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

private:
    static std::ofstream s_fileStream;
    static Level s_minLevel;
    static bool s_consoleOutput;

    static std::string GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    static std::string LevelToString(Level level) {
        switch (level) {
            case Level::DEBUG:   return "DEBUG";
            case Level::INFO:    return "INFO";
            case Level::WARNING: return "WARNING";
            case Level::ERROR:   return "ERROR";
            default:            return "UNKNOWN";
        }
    }

public:
    static void Init(const std::string& filename, Level minLevel = Level::INFO, bool consoleOutput = true) {
        s_fileStream.open(filename, std::ios::app);
        s_minLevel = minLevel;
        s_consoleOutput = consoleOutput;

        if (!s_fileStream.is_open()) {
            throw std::runtime_error("Failed to open log file: " + filename);
        }

        Log(Level::INFO, "Logger initialized");
    }

    static void Shutdown() {
        if (s_fileStream.is_open()) {
            s_fileStream.close();
        }
    }

    static void SetMinLevel(Level level) {
        s_minLevel = level;
    }

    static void SetConsoleOutput(bool enable) {
        s_consoleOutput = enable;
    }

    static void Log(Level level, const std::string& message) {
        if (level < s_minLevel) return;

        std::stringstream ss;
        ss << "[" << GetTimestamp() << "] "
           << "[" << LevelToString(level) << "] "
           << message;

        if (s_fileStream.is_open()) {
            s_fileStream << ss.str() << std::endl;
            s_fileStream.flush();
        }

        if (s_consoleOutput) {
            switch (level) {
                case Level::ERROR:
                    std::cerr << "\033[1;31m" << ss.str() << "\033[0m" << std::endl; // Rouge
                    break;
                case Level::WARNING:
                    std::cout << "\033[1;33m" << ss.str() << "\033[0m" << std::endl; // Jaune
                    break;
                default:
                    std::cout << ss.str() << std::endl;
                    break;
            }
        }
    }

    static void Debug(const std::string& message) {
        Log(Level::DEBUG, message);
    }

    static void Info(const std::string& message) {
        Log(Level::INFO, message);
    }

    static void Warning(const std::string& message) {
        Log(Level::WARNING, message);
    }

    static void Error(const std::string& message) {
        Log(Level::ERROR, message);
    }
};

inline std::ofstream Logger::s_fileStream;
inline Logger::Level Logger::s_minLevel = Logger::Level::INFO;
inline bool Logger::s_consoleOutput = true;
