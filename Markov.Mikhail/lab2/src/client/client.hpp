#pragma once

#include "../includes/includes.hpp"
#include "../connections/all_connections.hpp"

void client_signal_handler(int, siginfo_t *, void *);

template <Conn T>
class Client
{
private:
    int host_pid;
    int pid;
    // T host_to_client;
    // T client_to_host;
    // T host_to_client_general;
    // T client_to_host_general;
    std::vector<T> connections;
    struct sigaction signal_handler;
    Client(const std::string &host_pid_path, bool create)
    {
        std::fstream file(host_pid_path);
        if (!file)
            throw std::runtime_error("No config file" + std::string(std::filesystem::absolute(host_pid_path)));
        std::cout << "File opened successfully." << std::endl;
        file >> host_pid;
        std::cout << "Host PID: " << host_pid << std::endl;
        file.close();
        pid = getpid();

        signal_handler.sa_sigaction = client_signal_handler;
        signal_handler.sa_flags = SA_SIGINFO;

        if (sigaction(SIGUSR1, &signal_handler, nullptr) == -1)
        {
            throw std::runtime_error("Failed to register signal handler");
        }
        if (sigaction(SIGUSR2, &signal_handler, nullptr) == -1)
        {
            throw std::runtime_error("Failed to register signal handler");
        }

        kill(host_pid, SIGUSR1);

        sem_t *semaphore = sem_open((T::make_filename(host_pid, pid) + "_creation").c_str(), O_CREAT, 0777, 0);
        struct timespec tm;
        int s;
        if (clock_gettime(CLOCK_REALTIME, &tm) == -1)
            throw std::runtime_error("Failed to get real time");
        tm.tv_sec += 5;
        while ((s = sem_timedwait(semaphore, &tm)) == -1 && errno == EINTR)
            continue;

        connections.emplace_back(T::make_filename(host_pid, pid), create);
        connections.emplace_back(T::make_filename(pid, host_pid), create);
        connections.emplace_back(T::make_general_filename(host_pid, pid), create);
        connections.emplace_back(T::make_general_filename(pid, host_pid), create);

        sem_unlink(T::make_filename(host_pid, pid).c_str());
    }

public:
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;
    Client(Client &&) = delete;
    Client &operator=(Client &&) = delete;
    ~Client() = default;

    friend void client_signal_handler(int, siginfo_t *, void *);
    static Client& get_instance(const std::string &host_pid_path, bool create = false)
    {
        static Client instance(host_pid_path, create);
        return instance;
    }
    bool send_to_host(const std::string& message)
    {
        bool f = connections[1].Write(message);
        kill(host_pid, SIGUSR1);
        return f;
    }

    bool read_from_host(std::string& message)
    {
        return connections[0].Read(message);
    }

    bool send_to_host_general(const std::string &message)
    {
        bool f = connections[3].Write(message);
        kill(host_pid, SIGUSR2);
        return f;
    }

    bool read_from_host_general(std::string &message)
    {
        return connections[2].Read(message);
    }
};

namespace client_namespace
{
    const bool identifier = false;
    const std::filesystem::path host_pid_path = std::filesystem::current_path() / "host/host.txt";
}