#pragma once

#include "../includes/includes.hpp"

class Daemon
{
public:
    static Daemon &get_instance()
    {
        static Daemon instance;
        return instance;
    };
    void daemonize(const std::filesystem::path &);
    void run(...);

private:
    volatile sig_atomic_t got_sighup = 0;
    volatile sig_atomic_t got_sigterm = 0;

    std::filesystem::path pid_file;
    
    void create_pid_file();

    friend void signal_handler(int sig);

    Daemon() = default;
    Daemon(const Daemon &) = delete;
    Daemon(Daemon &&) = delete;
    Daemon &operator=(const Daemon &) = delete;
    Daemon &operator=(Daemon &&) = delete;
};
