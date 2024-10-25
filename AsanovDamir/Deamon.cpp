#include "Deamon.hpp"
#include "Logger.hpp"
#include "Config.hpp"

Deamon& Deamon::getInstance()
{
    static Deamon instance;
    return instance;
}

void Deamon::start()
{
    // TODO: implement
}
