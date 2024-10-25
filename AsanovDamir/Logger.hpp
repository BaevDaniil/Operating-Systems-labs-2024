#pragma once

class Logger
{
public:
    Logger& get_instance()
    {
        static Logger instance;
        return instance;
    }

    // TODO: implement

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};
