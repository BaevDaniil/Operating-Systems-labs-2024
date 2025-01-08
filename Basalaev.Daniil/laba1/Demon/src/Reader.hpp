#pragma once

#include <string>

inline std::string const CONFIG_PATH{"Demon/config.txt"};

class Reader
{
public:
    static Reader& getInstance();
    bool readConfig(std::string const& configPath = CONFIG_PATH);

    std::string const& getDir1() const noexcept { return m_dir1; }
    std::string const& getDir2() const noexcept { return m_dir2; }
    int getInterval() const noexcept { return m_interval; }

private:
    Reader() = default;
    Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;

    std::string m_configPath;
    std::string m_dir1;
    std::string m_dir2;
    int m_interval;
};
