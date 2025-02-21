#ifndef DEMON_H
#define DEMON_H

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <syslog.h>
#include <unistd.h>
#include <string>
#include <sys/stat.h>
#include <regex>
namespace fs = std::filesystem;

struct config_line
{
    fs::path from;
    fs::path to;
    std::string ext;
};

class Daemon
{
public: 
    static Daemon& get_instance()
    {
        static Daemon instance;
        return instance;
    }
    bool start(const std::string &config_file);
private:
    std::string config_file;
    fs::path current_path;
    std::vector<config_line> config ;
    const std::string IDENT = "demon";
    const std::string PID_FILE = "/var/run/" + IDENT + ".pid";
    std::string CONFIG_FILE;
    

    Daemon() = default;
    Daemon(const Daemon &) = delete;
    Daemon(Daemon &&) = delete;
    Daemon &operator=(const Daemon &) = delete;
    Daemon &operator=(Daemon &&) = delete;

    friend void sighup_fun(int sig);
};

#endif
