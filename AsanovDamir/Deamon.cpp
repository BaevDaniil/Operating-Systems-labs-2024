#include "Deamon.hpp"

Deamon& Deamon::get_instance()
{
    static Deamon instance;
    return instance;
}

void Deamon::start()
{
    // TODO: implement
}
