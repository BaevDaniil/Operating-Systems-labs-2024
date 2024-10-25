#pragma once

#include <string>

class Config
{
public:
    Config();
    bool read_config();

private:
    std::string get_absolute_path(const char* path);

    std::string pathToConfig;
    std::string folder1;
    std::string folder2;
    int seconds;
};
