#pragma once

#include <string>

class Config
{
public:
    Config();
    bool read_config();

    const std::string& get_folder1() const { return folder1; }
    const std::string& get_folder2() const { return folder2; }
    int get_seconds() const { return seconds; }

private:
    std::string get_absolute_path(const char* path);

    std::string pathToConfig;
    std::string folder1;
    std::string folder2;
    int seconds;
};
