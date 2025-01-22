#include "Logger.hpp"
#include <iostream>

Logger& Logger::getInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger() {}

void Logger::logInfo(std::string const& entity, std::string const& message) const
{
    std::cout << "INFO [" << entity << "]: " << message << std::endl;
}

void Logger::logError(std::string const& entity, std::string const& message) const
{
    std::cout << "ERROR [" << entity << "]: " << message << std::endl;
}
