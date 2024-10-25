#include "Config.hpp"
#include <fstream>

std::string Config::get_absolute_path(const char* path)
{
    char absolute_path_buffer[4096];
    if (!realpath(path, absolute_path_buffer)) return "";
    return absolute_path_buffer;
}

Config::Config() : pathToConfig{get_absolute_path("Config.txt")}
{}

bool Config::read_config()
{
    std::ifstream config(pathToConfig.c_str());
    if (!config) return false;

    std::getline(config, folder1);
    std::getline(config, folder2);
    config >> seconds;

    if (folder1.empty() || folder2.empty() || seconds == 0 || pathToConfig.empty()) return false;
    return true;
}

void Config::set_path_to_config(const char* path_to_config)
{
    pathToConfig = get_absolute_path(path_to_config);
}
