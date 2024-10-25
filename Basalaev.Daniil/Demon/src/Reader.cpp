#include "Reader.hpp"
#include <fstream>

Reader& Reader::getInstance()
{
    static Reader instance;
    return instance;
}

bool Reader::readConfig(std::string const& configPath)
{
    if (m_configPath.empty())
    {
        char absPath[4096];
        if (realpath(configPath.c_str(), absPath) == nullptr)
        {
            return false;
        }
        m_configPath = absPath;
    }

    std::ifstream configFile(m_configPath);
    if (!configFile) { return false; }

    std::getline(configFile, m_dir1);
    std::getline(configFile, m_dir2);
    configFile >> m_interval;

    return true;
}
