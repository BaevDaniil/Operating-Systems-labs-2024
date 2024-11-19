#pragma once

#include <vector>
#include <filesystem>
#include <chrono>
#include <csignal>
#include <thread>
#include "config.hpp"
#include "data.hpp"

class Daemon {
public:
    static Daemon& get_instance() {
        static Daemon instance; // Статический экземпляр
        return instance;
    }

    void run(const std::string& config_file);
    void reload_config();

    void set_sighup() { got_sighup = 1; }
    void set_sigterm() { got_sigterm = 1; }

private:
    Config* config; // Указатель на Config
    std::vector<Data> sync_data;
    std::chrono::time_point<std::chrono::steady_clock> last_sync_time;

    volatile sig_atomic_t got_sighup = 0;
    volatile sig_atomic_t got_sigterm = 0;

    void daemonize();
    void create_pid_file();
    void sync_folders();

    Daemon(); // Конструктор
    Daemon(const Daemon &) = delete;
    Daemon(Daemon &&) = delete;
    Daemon &operator=(const Daemon &) = delete;
    Daemon &operator=(Daemon &&) = delete;
};
