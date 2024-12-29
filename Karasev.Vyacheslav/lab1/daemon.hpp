#pragma once

#include <string>
#include <vector>
#include <syslog.h>
#include <signal.h>

class Daemon {
public:
    static Daemon& getInstance();
    void start();
    void stop();
    void reloadConfig();
    void setConfigFilePath(const std::string& path);

private:
    Daemon();
    ~Daemon();
    Daemon(const Daemon&) = delete;
    Daemon& operator=(const Daemon&) = delete;

    void daemonize();
    void handleSignals();
    void readConfig();
    void processDirectories();
    void checkAndTerminateExistingDaemon();
    void writePidFile();
    void removePidFile();
    void removeDirectoryContents(const std::string& path);


    std::string configFilePath;
    std::vector<std::pair<std::string, std::string>> directories;
    int interval;
    pid_t pid;
    std::string pidFilePath;
    std::string currentWorkingDir;
};
