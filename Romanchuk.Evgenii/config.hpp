#pragma once

#include <string>
#include <vector>
#include <syslog.h>
#include "data.hpp"

class Config {
public:
    explicit Config(const std::string& config_path) : path(config_path) {}

    std::vector<Data> read(); // Объявление метода

private:
    std::string path; // Объявление переменной path
};
