#pragma once

#include <string>
#include <syslog.h>

enum class Status : short
{
INFO = LOG_INFO,
ERROR = LOG_ERR
};

class Logger
{
public:
    static Logger& get_instance()
    {
        static Logger instance;
        return instance;
    }

    void open_log()
    {
        openlog("Deamon", LOG_PID, LOG_DAEMON);
    }
    void log(Status status, const std::string& message)
    {
        syslog(static_cast<int>(status), "%s", message.c_str());
    }
    void close_log() { closelog(); }

private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};
