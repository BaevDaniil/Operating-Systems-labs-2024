#pragma once

#include <string>
#include <chrono>

class DeepCleanerDaemon
{
private:
    const std::string pidPath = "/var/run/deep_deletion_daemon.pid";
    const int depth = 3;
    const std::chrono::seconds defaultSleepDuration = std::chrono::seconds(10);

    std::string configAbsPath;
    std::string targetDirPath;
    std::chrono::seconds sleepDuration;
    bool isTerminateReceived = false;

    DeepCleanerDaemon() {};
    DeepCleanerDaemon(const DeepCleanerDaemon &) = delete;
    DeepCleanerDaemon &operator=(const DeepCleanerDaemon &) = delete;

    void WritePid();
    void Daemonize();
    void KillExistingDaemon();

public:
    static DeepCleanerDaemon &Get()
    {
        static DeepCleanerDaemon instance;
        return instance;
    }

    void Initialize(const std::string& configLocalPath);

    // SIGTERM handler
    void Terminate();

    // SIGHUP handler
    void ReloadConfig();

    void Run();
};