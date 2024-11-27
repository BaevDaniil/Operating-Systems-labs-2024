#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include "Config.h"
#include "PathedConfigRule.h"

class Daemon {
private:
    const std::string pidFilePath = "/var/run/app_daemon.pid";
    std::vector<ConfigRule> rules;
    Config config;

    // Singleton
    Daemon() = default;
    ~Daemon() = default;
    Daemon(Daemon const& other) = default;
    Daemon& operator = (Daemon const& other) = default;

    void forkFromParent();
    int safeOpenPidFile();
    void checkRunningInstance(int pidFile);
    void terminateExistingProcess(int pidFile);
    void preparePidFile();
    void saveConfigRules();
    bool copyFiles(PathedConfigRule const& rule);
    PathedConfigRule ruleToPathed(ConfigRule const& rule, std::filesystem::path const& initDir);
    bool setupFolders(PathedConfigRule const& rule);
public:
    static Daemon& getInstance() {
        static Daemon daemon;
        return daemon;
    }

    void run(
        std::string const& path,
        std::filesystem::path const& initDir,
        int repeat = 60
    );

    void handleSighup();
    void handleSigterm();
};