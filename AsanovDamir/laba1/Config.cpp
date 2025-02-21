#include "Config.hpp"
#include <fstream>
#include <filesystem>

std::string Config::get_absolute_path(const char* pathStr)
{
    std::filesystem::path absPath = std::filesystem::absolute(pathStr);
    if (std::filesystem::exists(absPath))
        return absPath.c_str();
    else
        return "";

    return std::filesystem::absolute(pathStr).c_str();
}

Config::Config() : pathToConfig{get_absolute_path("Config.txt")}
{}

bool Config::read_config()
{
    std::ifstream config(pathToConfig.c_str());
    if (!config) return false;

    std::getline(config, folder1);
    folder1 = get_absolute_path(folder1.c_str());
    std::getline(config, folder2);
    folder2 = get_absolute_path(folder2.c_str());
    config >> seconds;

    if (folder1.empty() || folder2.empty() || seconds == 0 || pathToConfig.empty()) return false;
    return true;
}

void Config::set_path_to_config(const char* path_to_config)
{
    pathToConfig = get_absolute_path(path_to_config);
}
