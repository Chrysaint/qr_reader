#include "logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>

Logger::Level Logger::current_level_ = Logger::INFO;

void Logger::setLogLevel(Level level) {
    current_level_ = level;
    log(INFO, "Log level set to: " + levelToString(level));
}

void Logger::log(Level level, const std::string& message) {
    if (level >= current_level_) {
        printLog(level, message);
    }
}

void Logger::debug(const std::string& message) {
    log(DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(INFO, message);
}

void Logger::warning(const std::string& message) {
    log(WARNING, message);
}

void Logger::error(const std::string& message) {
    log(ERROR, message);
}

void Logger::logQRDetection(const std::string& qrData, bool success) {
    if (success) {
        log(INFO, "QR code detected successfully: " + qrData);
    } else {
        log(WARNING, "QR code detection failed");
    }
}

void Logger::startOperation(const std::string& operation) {
    log(DEBUG, "Starting: " + operation);
}

void Logger::endOperation(const std::string& operation) {
    log(DEBUG, "Completed: " + operation);
}

std::string Logger::levelToString(Level level) {
    switch (level) {
        case DEBUG:   return "DEBUG";
        case INFO:    return "INFO";
        case WARNING: return "WARNING";
        case ERROR:   return "ERROR";
        default:      return "UNKNOWN";
    }
}

std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    return ss.str();
}

void Logger::printLog(Level level, const std::string& message) {
    std::string levelStr = levelToString(level);
    std::string timeStr = getCurrentTime();

    std::string colorCode = "";
    std::string resetCode = "";

    #ifdef __unix__
    switch (level) {
        case DEBUG:   colorCode = "\033[36m"; break; // Cyan
        case INFO:    colorCode = "\033[32m"; break; // Green
        case WARNING: colorCode = "\033[33m"; break; // Yellow
        case ERROR:   colorCode = "\033[31m"; break; // Red
        default:      colorCode = "";
    }
    resetCode = "\033[0m";
    #endif

    std::cout << "[" << timeStr << "] "
              << colorCode << "[" << levelStr << "]" << resetCode
              << " " << message << std::endl;
}
