#include <string>
#include <vector>
#include <syslog.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <csignal>
#include <cstdlib>
#include <dirent.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;

class Daemon {
public:
    static Daemon& getInstance() {
        static Daemon instance;
        return instance;
    }

    void setConfigFilePath(const std::string& path) {
        configFilePath = path;
    }

    void reloadConfig() {
        syslog(LOG_INFO, "Reloading configuration file");
        readConfigFile();
    }

    void stop() {
        syslog(LOG_INFO, "Daemon is stopping");
        closelog();
        removePidFile();
        exit(0);
    }

    void start() {
        checkAndTerminateExistingDaemon();
        daemonize();
        handleSignals();
        readConfigFile();
        writePidFile();

        while (true) {
            for (const auto& config : folderConfigs) {
                processFolder(config.folderPath, config.extension);
            }
            sleep(interval);
        }
    }

    Daemon(const Daemon&) = delete;
    Daemon& operator=(const Daemon&) = delete;

private:
    Daemon() : interval(30), pid(0), pidFilePath("/tmp/daemon.pid") {
        openlog("daemon", LOG_PID | LOG_CONS, LOG_DAEMON);
    }

    ~Daemon() {
        closelog();
        removePidFile();
    }

    void removePidFile() {
        std::remove(pidFilePath.c_str());
    }

    void daemonize() {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            currentWorkingDir = std::string(cwd);
        }
        else {
            syslog(LOG_ERR, "Failed to get current working directory");
        }

        pid = fork();
        if (pid < 0) {
            syslog(LOG_ERR, "Fork failed");
            exit(1);
        }
        if (pid > 0) {
            exit(0);
        }

        umask(0);
        setsid();

        if (chdir("/") < 0) {
            syslog(LOG_ERR, "Chdir failed");
            exit(1);
        }

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        open("/dev/null", O_RDONLY);
        open("/dev/null", O_RDWR);
        open("/dev/null", O_RDWR);
    }

    void handleSignals() {
        signal(SIGHUP, [](int) { Daemon::getInstance().reloadConfig(); });
        signal(SIGTERM, [](int) { Daemon::getInstance().stop(); });
    }

    void readConfigFile() {
        std::ifstream configFile(configFilePath);
        if (!configFile.is_open()) {
            syslog(LOG_ERR, "Failed to open config file: %s", configFilePath.c_str());
            exit(1);
        }

        std::string line;
        while (std::getline(configFile, line)) {
            std::istringstream iss(line);
            std::string folderPath, extension;
            if (iss >> folderPath >> extension) {
                folderConfigs.push_back({folderPath, extension});
            }
        }

        configFile.close();
    }

    void processFolder(const std::string& folderPath, const std::string& extension) {
        try {
            for (const auto& entry : fs::directory_iterator(folderPath)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (filename.substr(filename.find_last_of(".") + 1) == extension) {
                        logFileDetails(folderPath, filename);
                    }
                }
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Failed to open directory: " << folderPath << " - " << e.what() << std::endl;
        }
    }

    void logFileDetails(const std::string& folderPath, const std::string& filename) {
        std::string filePath = folderPath + "/" + filename;
        struct stat fileStat;
        if (stat(filePath.c_str(), &fileStat) == 0) {
            char timeBuffer[80];
            struct tm *timeInfo = localtime(&fileStat.st_mtime);
            strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeInfo);

            syslog(LOG_INFO, "Folder: %s, File: %s, Size: %ld bytes, Last Modified: %s",
                folderPath.c_str(), filename.c_str(), fileStat.st_size, timeBuffer);
        } else {
            syslog(LOG_ERR, "Failed to get stats for file: %s", filePath.c_str());
        }
    }

    void checkAndTerminateExistingDaemon() {
        std::ifstream pidFile(pidFilePath);
        if (pidFile.is_open()) {
            pid_t existingPid;
            pidFile >> existingPid;
            pidFile.close();

            if (kill(existingPid, 0) == 0) {
                syslog(LOG_INFO, "Terminating existing daemon with PID %d", existingPid);
                kill(existingPid, SIGTERM);
                sleep(1);
            }
        }
    }

    void writePidFile() {
        std::ofstream pidFile(pidFilePath);
        if (pidFile.is_open()) {
            pidFile << getpid();
            pidFile.close();
        }
        else {
            syslog(LOG_ERR, "Failed to write PID file");
        }
    }

    struct FolderConfig {
        std::string folderPath;
        std::string extension;
    };

    std::vector<FolderConfig> folderConfigs;
    std::string configFilePath;
    int interval;
    pid_t pid;
    std::string pidFilePath;
    std::string currentWorkingDir;
};