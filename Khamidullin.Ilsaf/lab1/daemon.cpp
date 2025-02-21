#include "daemon.hpp"
#include <limits.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <csignal>
#include <fstream>
#include <fcntl.h>
#include <filesystem>
#include <queue>
#include <thread>

static void DeepCleanerDaemon::handleSignal(int signal)
{
    switch (signal)
    {
    case SIGTERM:
        syslog(LOG_INFO, "Daemon process terminated");
        DeepCleanerDaemon::Get().Terminate();
        break;
    case SIGHUP:
        syslog(LOG_INFO, "Reloading configuration");
        DeepCleanerDaemon::Get().ReloadConfig();
        break;
    default:
        break;
    }
}

void DeepCleanerDaemon::deleteDirectoriesByDepth(const std::string& rootPathStr, int depth)
{
    std::filesystem::path rootPath(rootPathStr);

    if (depth < 0) {
        syslog(LOG_ERR, "Error: depth cannot be negative.");
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(rootPath)) {
        if (std::filesystem::is_directory(entry)) {
            if (depth > 0) {
                deleteDirectoriesByDepth(entry.path().string(), depth - 1);
            }
            syslog(LOG_INFO, "Removing directory: %s", entry.path().c_str());
            std::filesystem::remove_all(entry.path());
        }
    }
}

void DeepCleanerDaemon::KillExistingDaemon()
{
    syslog(LOG_INFO, "Checking for any existing daemon processes");
    std::ifstream pidFile(pidPath);
    if (pidFile.is_open())
    {
        int runningDaemonPid = 0;
        if (pidFile >> runningDaemonPid && kill(runningDaemonPid, 0) == 0)
        {
            syslog(LOG_WARNING, "Stopping the existing daemon with PID: %i", runningDaemonPid);
            kill(runningDaemonPid, SIGTERM);
        }
    }
}

void DeepCleanerDaemon::WritePid()
{
    syslog(LOG_INFO, "Writing PID to file");
    std::ofstream pidFile(pidPath.c_str());
    if (!pidFile.is_open())
    {
        syslog(LOG_ERR, "Error writing PID to file");
        exit(EXIT_FAILURE);
    }
    pidFile << getpid();
    syslog(LOG_INFO, "PID written successfully");
}

void DeepCleanerDaemon::Daemonize()
{
    syslog(LOG_INFO, "Starting daemonization process");

    pid_t processId = fork();
    syslog(LOG_INFO, "Forked process with PID: %i", processId);
    if (processId < 0)
    {
        syslog(LOG_ERR, "Error during fork");
        exit(EXIT_FAILURE);
    }
    if (processId > 0)
    {
        exit(EXIT_SUCCESS);
    }

    umask(0);
    if (setsid() < 0)
    {
        syslog(LOG_ERR, "Error setting group ID");
        exit(EXIT_FAILURE);
    }
    if (chdir("/") < 0)
    {
        syslog(LOG_ERR, "Error switching to root directory");
        exit(EXIT_FAILURE);
    }
    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; --x)
    {
        close(x);
    }

    int devNull = open("/dev/null", O_RDWR);
    dup2(devNull, STDIN_FILENO);
    dup2(devNull, STDOUT_FILENO);
    dup2(devNull, STDERR_FILENO);
    syslog(LOG_INFO, "Daemonization process completed");
}

void DeepCleanerDaemon::Initialize(const std::string& configLocalPath)
{
    configAbsPath = std::filesystem::absolute(configLocalPath);

    openlog("DeepCleanerDaemon", LOG_NDELAY | LOG_PID | LOG_PERROR, LOG_USER);
    syslog(LOG_INFO, "Initializing daemon");

    KillExistingDaemon();
    Daemonize();

    syslog(LOG_INFO, "Setting up signal handlers");
    std::signal(SIGHUP, handleSignal);
    std::signal(SIGTERM, handleSignal);

    WritePid();

    ReloadConfig();
    syslog(LOG_INFO, "Daemon initialization finished");
}

void DeepCleanerDaemon::Run()
{
    while (!isTerminateReceived)
    {
        if (!std::filesystem::is_directory(targetDirPath))
        {
            syslog(LOG_WARNING, "Warning: Target directory path from config does not exist, continuing to wait");
        }
        else
        {
            deleteDirectoriesByDepth(targetDirPath, depth);
        }
        std::this_thread::sleep_for(sleepDuration);
    }
}

void DeepCleanerDaemon::Terminate()
{
    isTerminateReceived = true;
    closelog();
    syslog(LOG_INFO, "Daemon termination initiated");
}

void DeepCleanerDaemon::ReloadConfig()
{
    std::ifstream configFile(configAbsPath);
    if (!configFile.is_open())
    {
        syslog(LOG_ERR, "Error: Invalid config file");
        exit(EXIT_FAILURE);
    }
    if (!std::getline(configFile, targetDirPath))
    {
        syslog(LOG_ERR, "Error: Cannot read target directory path from config file");
        exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO, "Target directory path set to \"%s\"", targetDirPath.c_str());
    
    int sleepTimeSeconds = -1;
    if (!(configFile >> sleepTimeSeconds))
    {
        syslog(LOG_WARNING, "Warning: Cannot read sleep time from config file, using default value");
        sleepDuration = defaultSleepDuration;
    }
    else if (sleepTimeSeconds <= 0)
    {
        syslog(LOG_WARNING, "Warning: Sleep time from config file is not positive, using default value");
        sleepDuration = defaultSleepDuration;
    }
    else
    {
        sleepDuration = std::chrono::seconds(sleepTimeSeconds);
    }
    syslog(LOG_INFO, "Sleep time set to %li seconds", static_cast<long>(sleepDuration.count()));
}