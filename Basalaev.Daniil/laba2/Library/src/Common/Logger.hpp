#pragma once

#include <string>

inline constexpr auto HOST_LOG = "HOST";
inline constexpr auto CLIENT_LOG = "CLIENT";

#define LOG_INFO(entity, message) Logger::getInstance().logInfo(entity, message);
#define LOG_ERROR(entity, message) Logger::getInstance().logError(entity, message);

class Logger
{
public:
    static Logger& getInstance();

    void logInfo(std::string const& entity, std::string const& message) const;
    void logError(std::string const& entity, std::string const& message) const;

private:
    Logger();
    Logger(Logger const&) = delete;
    Logger& operator=(Logger const&) = delete;
};
