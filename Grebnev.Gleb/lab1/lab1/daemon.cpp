#include <iostream>
#include <fstream>
#include <filesystem>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <csignal>

class Daemon {
private:
    static Daemon* instance;
    std::string configPath;
    pid_t pid;
    std::vector<std::pair<std::string, size_t>> folders;

    Daemon() : pid(getpid()) {
        openlog("Daemon", LOG_PID, LOG_DAEMON);
    }

    void loadConfig() {
        folders.clear();
        std::ifstream configFile(configPath);
        if (configFile.is_open()) {
            std::string folder;
            size_t maxSize;
            while (configFile >> folder >> maxSize) {
                folders.emplace_back(folder, maxSize);
            }
            syslog(LOG_INFO, "Config loaded successfully.");
        }
        else {
            syslog(LOG_ERR, "Failed to open config file.");
        }
    }

    size_t getFolderSize(const std::string& path) {
        size_t totalSize = 0;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                totalSize += entry.file_size();
            }
        }
        return totalSize;
    }

    void cleanupFolders() {
        for (const auto& [folder, maxSize] : folders) {
            size_t currentSize = getFolderSize(folder);
            if (currentSize > maxSize) {
                for (const auto& entry : std::filesystem::directory_iterator(folder)) {
                    if (entry.is_regular_file()) {
                        std::filesystem::remove(entry.path());
                    }
                }
                syslog(LOG_INFO, "Folder %s cleared due to exceeding size limit.", folder.c_str());
            }
        }
    }

public:
    static Daemon& getInstance() {
        if (!instance) {
            instance = new Daemon();
        }
        return *instance;
    }

    void init(const std::string& path) {
        configPath = path;
        loadConfig();
    }

    void run() {
        while (true) {
            cleanupFolders();
            sleep(10);
        }
    }

    void reloadConfig(int) {
        loadConfig();
    }

    void terminate(int) {
        syslog(LOG_INFO, "Daemon exiting.");
        closelog();
        exit(0);
    }
};

Daemon* Daemon::instance = nullptr;

void signalHandler(int signal) {
    Daemon& daemon = Daemon::getInstance();
    if (signal == SIGHUP) {
        daemon.reloadConfig(signal);
    }
    else if (signal == SIGTERM) {
        daemon.terminate(signal);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_path>" << std::endl;
        return 1;
    }

    Daemon& daemon = Daemon::getInstance();
    daemon.init(argv[1]);

    pid_t pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "Fork failed.");
        return 1;
    }
    else if (pid > 0) {
        return 0;
    }

    umask(0);
    setsid();
    chdir("/");

    std::ofstream pidFile("/var/run/daemon_example.pid");
    if (!pidFile.is_open()) {
        syslog(LOG_ERR, "Failed to open PID file.");
        return 1;
    }
    pidFile << getpid();
    pidFile.close();

    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);

    daemon.run();

    return 0;
}