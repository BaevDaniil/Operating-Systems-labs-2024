#pragma once

#include <vector>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <iostream>
#include <csignal>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <exception>

class Daemon
{
public:
    static Daemon &get_instance()
    {
        static Daemon instance;
        return instance;
    };
    void run(...);

private:
    std::vector<std::chrono::time_point<std::chrono::steady_clock>> time_points;

    volatile sig_atomic_t got_sighup = 0;
    volatile sig_atomic_t got_sigterm = 0;

    void create_pid_file();
    void daemonize();

    friend void signal_handler(int sig);

    Daemon() = default;
    Daemon(const Daemon &) = delete;
    Daemon(Daemon &&) = delete;
    Daemon &operator=(const Daemon &) = delete;
    Daemon &operator=(Daemon &&) = delete;
};
