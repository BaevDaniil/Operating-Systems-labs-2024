#include "daemon.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <syslog.h>
#include <cstring>
#include <cerrno>
#include <fstream>
#include <chrono>
#include <stdexcept>
#include <sstream>

void signal_handler(int sig) {
    Daemon& daemon = Daemon::get_instance();
    if (sig == SIGHUP) {
        daemon.set_sighup();
    } else if (sig == SIGTERM) {
        daemon.set_sigterm();
    }
}

Daemon::Daemon() : config(nullptr) {} // Инициализация указателя

#include <signal.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <string>

void Daemon::daemonize() {
    const char* pid_file = "/var/run/daemon.pid";
    int pid_file_handle = open(pid_file, O_RDWR | O_CREAT, 0600);

    if (pid_file_handle == -1) {
        syslog(LOG_ERR, "Cannot open PID file %s: %s", pid_file, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Проверка, заблокирован ли файл, что указывает на запущенный процесс демона
    if (lockf(pid_file_handle, F_TLOCK, 0) == -1) {
        syslog(LOG_ERR, "Daemon is already running. Exiting new instance.");
        exit(EXIT_SUCCESS); // Выход, если демон уже запущен
    }

    // Если файл существует, читаем PID
    std::ifstream pid_file_stream(pid_file);
    if (pid_file_stream.is_open()) {
        pid_t existing_pid;
        pid_file_stream >> existing_pid;
        pid_file_stream.close();

        // Проверка, существует ли процесс с указанным PID
        if (existing_pid > 0 && kill(existing_pid, 0) == 0) {
            syslog(LOG_ERR, "Daemon already running with PID %d. Exiting.", existing_pid);
            exit(EXIT_SUCCESS);
        }
    }

    // Создаем новый процесс и отделяем его, как и раньше
    pid_t pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "Fork failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        exit(EXIT_SUCCESS); // Родительский процесс завершает работу
    }

    // Отделение от терминала
    umask(0);
    if (setsid() < 0) {
        syslog(LOG_ERR, "Failed to create new session: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (chdir("/") < 0) {
        syslog(LOG_ERR, "Failed to change directory to root: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Записываем PID текущего процесса в PID-файл
    char str[10];
    snprintf(str, sizeof(str), "%d\n", getpid());
    write(pid_file_handle, str, strlen(str));

    close(pid_file_handle);
}



void Daemon::create_pid_file() {
    const char* pid_file = "/var/run/daemon.pid";
    int pid_file_handle = open(pid_file, O_RDWR | O_CREAT, 0600);
    if (pid_file_handle == -1) {
        syslog(LOG_ERR, "Cannot open PID file %s: %s", pid_file, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (lockf(pid_file_handle, F_TLOCK, 0) == -1) {
        syslog(LOG_ERR, "Daemon is already running.");
        exit(EXIT_FAILURE);
    }

    char str[10];
    snprintf(str, sizeof(str), "%d\n", getpid());
    write(pid_file_handle, str, strlen(str));

    close(pid_file_handle);
}

void Daemon::run(const std::string& config_file) {
    daemonize();
    config = new Config(config_file); // Создание нового экземпляра Config
    sync_data = config->read(); 
    last_sync_time = std::chrono::steady_clock::now();

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    while (true) {
        if (got_sighup) {
            got_sighup = 0;
            syslog(LOG_INFO, "Reloading config file.");
            reload_config();
        }

        if (got_sigterm) {
            syslog(LOG_INFO, "Daemon exiting.");
            delete config; // Освобождение памяти
            exit(EXIT_SUCCESS);
        }

        auto now = std::chrono::steady_clock::now();
        int elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_sync_time).count();
        if (elapsed >= sync_data[0].time) {
            sync_folders();
            last_sync_time = now;
        }

        // std::this_thread::sleep_for(std::chrono::seconds(sync_data[0].time));
        sleep(1);
    }
}

void Daemon::reload_config() {
    delete config; // Освобождение памяти
    config = new Config("config.txt"); // Замените на реальный путь
    sync_data = config->read(); // Обновляем sync_data
}

void Daemon::sync_folders() {
    std::filesystem::path src_dir(sync_data[0].folder1);
    std::filesystem::path dest_dir(sync_data[0].folder2);

    // Проверка существования обеих папок
    if (!std::filesystem::exists(src_dir)) {
        syslog(LOG_ERR, "Source folder does not exist: %s", src_dir.c_str());
        return;
    }
    if (!std::filesystem::exists(dest_dir)) {
        syslog(LOG_ERR, "Destination folder does not exist: %s", dest_dir.c_str());
        return;
    }

    // Лямбда для копирования файлов в заданном направлении
    auto sync_directory = [](const std::filesystem::path& from_dir, const std::filesystem::path& to_dir) {
        try {
            for (const auto& entry : std::filesystem::directory_iterator(from_dir)) {
                const auto& from_path = entry.path();
                auto to_path = to_dir / from_path.filename();

                if (!std::filesystem::exists(to_path) || 
                    std::filesystem::last_write_time(from_path) > std::filesystem::last_write_time(to_path)) {
                    std::filesystem::copy(from_path, to_path, std::filesystem::copy_options::overwrite_existing);
                    syslog(LOG_INFO, "Copied %s to %s", from_path.c_str(), to_path.c_str());
                }
            }
        } catch (const std::exception& e) {
            syslog(LOG_ERR, "Error during synchronization: %s", e.what());
        }
    };

    // Синхронизация в обоих направлениях
    syslog(LOG_INFO, "Starting synchronization between %s and %s", src_dir.c_str(), dest_dir.c_str());
    
    sync_directory(src_dir, dest_dir);
    sync_directory(dest_dir, src_dir);

    syslog(LOG_INFO, "Completed synchronization between %s and %s", src_dir.c_str(), dest_dir.c_str());
}
