#pragma once

#include "Logger.hpp"
#include "Config.hpp"

class Deamon
{
public:
    static Deamon& get_instance();
    void start();

private:
    Deamon() = default;
    Deamon(const Deamon&) = delete;
    Deamon& operator=(const Deamon&) = delete;
};
