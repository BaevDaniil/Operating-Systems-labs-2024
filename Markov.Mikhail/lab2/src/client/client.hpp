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

        kill(host_pid, SIGUSR1);

        const std::string filename_dir = "/tmp/" + T::to_string() + '/';
        connections.emplace_back(filename_dir + std::to_string(host_pid) + "_" + std::to_string(pid), create);
        connections.emplace_back(filename_dir + std::to_string(pid) + '_' + std::to_string(host_pid), create);
        connections.emplace_back(filename_dir + std::to_string(host_pid) + '_' + std::to_string(pid) + "_" + "general", create);
        connections.emplace_back(filename_dir + std::to_string(pid) + '_' + std::to_string(host_pid) + "_" + "general", create);
    }

public:
    friend void client_signal_handler(int, siginfo_t *, void *);
    static Client& get_instance(const std::string &host_pid_path, bool create = false)
    {
        static Client instance(host_pid_path, create);
        return instance;
    }
    void send_to_host(const std::string& message)
    {
        connections[1].Write(message);
        kill(host_pid, SIGUSR1);
    }

    void read_from_host(std::string& message)
    {
        connections[0].Read(message);
    }

    void send_to_host_general(const std::string &message)
    {
        connections[3].Write(message);
        kill(host_pid, SIGUSR2);
    }

    void read_from_host_general(std::string &message)
    {
        connections[2].Read(message);
    }
};
