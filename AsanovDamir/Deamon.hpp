#pragma once

#include "Logger.hpp"
#include "Config.hpp"

class Deamon
{
public:
    static Deamon& get_instance();
    void start();
    bool coping();

private:
    Deamon() = default;
    Deamon(const Deamon&) = delete;
    Deamon& operator=(const Deamon&) = delete;

    void handle_signals();
    static void sighup_handler(int signum);
    static void sigterm_handler(int signum);

    void daemonize();
    bool crate_pid_file();
    void destroy_prev_deamon_if_exist();

    static Config config;
};