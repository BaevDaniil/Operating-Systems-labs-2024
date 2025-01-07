#include "Deamon.hpp"
#include <signal.h>
#include <sys/stat.h>
#include <filesystem>
#include <fstream>

Config Deamon::config;

const char* PID = "/var/run/Deamon.pid";

Deamon& Deamon::get_instance()
{
    static Deamon instance;
    return instance;
}

void Deamon::destroy_prev_deamon_if_exist()
{
    if (std::ifstream pid_file(PID); pid_file) {
        pid_t pid;
        pid_file >> pid;
        if (kill(pid, 0) == 0) {
            Logger::get_instance().log(Status::INFO, "Deamon is already running. Killing the old instance...");
            if (kill(pid, SIGTERM) != 0) {
                Logger::get_instance().log(Status::ERROR, "Failed to terminate the existing daemon process.");
                unlink(PID);
            }
        }
    }
}

bool Deamon::crate_pid_file()
{
    std::ofstream new_pid_file(PID);
    if (!new_pid_file) {
        Logger::get_instance().log(Status::ERROR, "Failed to create PID file.");
        return false;
    }

    new_pid_file << getpid();
    return true;
}

void Deamon::daemonize()
{
    pid_t pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS);

    if (setsid() < 0)
        exit(EXIT_FAILURE);

    signal(SIGHUP, SIG_IGN);

    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS);

    umask(0);
    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

bool Deamon::coping()
{
    namespace fs = std::filesystem;

    const std::string& folder1 = config.get_folder1();
    const std::string& folder2 = config.get_folder2();

    const std::string imgDir = folder2 + "/IMG";
    const std::string othersDir = folder2 + "/OTHERS";

    try {
        // Clear folder 2
        if (fs::exists(folder2)) {
            for (const auto& entry : fs::directory_iterator(folder2)) {
                fs::remove_all(entry.path());
            }
        } else {
            fs::create_directories(folder2);
        }

        // Craate IMG and OTHERS folders
        fs::create_directories(imgDir);
        fs::create_directories(othersDir);

        // coping
        for (const auto& entry : fs::directory_iterator(folder1)) {
            if (entry.is_regular_file()) {
                std::string fileName = entry.path().filename().string();
                std::string destPath;
                if (entry.path().extension() == ".png") {
                    destPath = imgDir + "/" + fileName;
                } else {
                    destPath = othersDir + "/" + fileName;
                }
                fs::copy(entry.path(), destPath);
            }
        }
    } catch (const std::exception& ex) {
        Logger::get_instance().log(Status::ERROR, "Error during copying: " + std::string(ex.what()));
        return false;
    }

    Logger::get_instance().log(Status::INFO, "Copied files from" + folder1 + " to " + folder2);
    return true;
}

void Deamon::start(const char* config_path)
{
    config.set_path_to_config(config_path);

    destroy_prev_deamon_if_exist();
    daemonize();

    Logger::get_instance().open_log();
    Logger::get_instance().log(Status::INFO, "Start deamon");

    if (!config.read_config()) {
        
        Logger::get_instance().log(Status::ERROR, std::string("Failed to read config"));
        exit(EXIT_FAILURE);
    }

    if (!crate_pid_file())
        exit(EXIT_FAILURE);

    handle_signals();

    while (true) {
        coping();
        sleep(config.get_seconds());
    }
}

void Deamon::handle_signals()
{
    signal(SIGHUP, sighup_handler);
    signal(SIGTERM, sigterm_handler);
}

void Deamon::sighup_handler(int)
{
    Logger::get_instance().log(Status::INFO, "Re-reading config");
    if (!config.read_config()) {
        Logger::get_instance().log(Status::ERROR, "Failed to read config");
        Logger::get_instance().close_log();
        unlink(PID);
        exit(EXIT_FAILURE);
    }
}

void Deamon::sigterm_handler(int)
{
    Logger::get_instance().log(Status::INFO, "Terminating");
    Logger::get_instance().close_log();
    unlink(PID);
    exit(0);
}
