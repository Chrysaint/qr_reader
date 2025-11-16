#ifndef QR_READER_LOGGER_H
#define QR_READER_LOGGER_H

#include <string>
#include <iostream>

class Logger {
public:
    enum Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    static void setLogLevel(Level level);

    static void log(Level level, const std::string& message);

    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);

    static void logQRDetection(const std::string& qrData, bool success);

    static void startOperation(const std::string& operation);
    static void endOperation(const std::string& operation);

private:
    static Level current_level_;

    static std::string levelToString(Level level);
    static std::string getCurrentTime();
    static void printLog(Level level, const std::string& message);
};

#endif // QR_READER_LOGGER_H
