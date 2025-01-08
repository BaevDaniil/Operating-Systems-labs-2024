#include "Reader.hpp"
#include <fstream>
#include <filesystem>

std::string getAbsPath(std::string const& path)
{
    char absPath[4096];
    if (realpath(path.c_str(), absPath) == nullptr) { return ""; }
    return absPath;
}

Reader& Reader::getInstance()
{
    static Reader instance;
    return instance;
}

bool Reader::readConfig(std::string const& configPath)
{
    // set config path
    if (m_configPath.empty())
    {
        m_configPath = getAbsPath(configPath);
        if (m_configPath.empty()) { return false; }
    }

    // open config
    std::ifstream configFile(m_configPath);
    if (!configFile) { return false; }

    static auto setDir = [](std::string& dir)
    {
        static auto currentDir = std::filesystem::current_path();

        if (auto pathDir1 = std::filesystem::path(dir); pathDir1.is_absolute() && std::filesystem::is_directory(pathDir1))
        {
            dir = pathDir1.c_str();
        }
        else if (auto fullPathDir1 = currentDir / pathDir1; std::filesystem::is_directory(fullPathDir1))
        {
            dir = fullPathDir1.c_str();
        }
        else
        {
            return false;
        }
        return true;
    };

    // set dirs
    std::getline(configFile, m_dir1);
    if (!setDir(m_dir1)) { return false; }

    std::getline(configFile, m_dir2);
    if (!setDir(m_dir2)) { return false; }

    configFile >> m_interval;
    if (m_interval <= 0) { return false; }

    return true;
}
