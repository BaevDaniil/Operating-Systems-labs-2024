#pragma once

#include "../connections/connection_fabric.hpp"

void host_signal_handler(int, siginfo_t *, void *);

class HostLogic
{
private:
    int client_pid;
    int pid;
    std::unique_ptr<Connection> host_to_client;
    std::unique_ptr<Connection> client_to_host;
    struct sigaction signal_handler;
    HostLogic(ConnectionType type, int host_pid, int client_pid) : client_pid(client_pid), pid(host_pid)
    {
        signal_handler.sa_sigaction = host_signal_handler;
        signal_handler.sa_flags = SA_SIGINFO;

        if (sigaction(SIGUSR1, &signal_handler, nullptr) == -1)
            throw std::runtime_error("Failed to register signal handler");

        host_to_client = std::move(ConnectionFabric::create_connection(type, ConnectionFabric::create_filename(type, pid, client_pid), true));
        client_to_host = std::move(ConnectionFabric::create_connection(type, ConnectionFabric::create_filename(type, client_pid, pid), true));
        sem_t *semaphore = sem_open(("sem" + ConnectionFabric::create_filename(type, pid, client_pid)).c_str(), O_CREAT, 0777, 0);
        if (semaphore == SEM_FAILED)
            throw std::runtime_error("Error to create a semaphore!");
        sem_post(semaphore);
    }

public:
    HostLogic(const HostLogic &) = delete;
    HostLogic &operator=(const HostLogic &) = delete;
    HostLogic(HostLogic &&) = delete;
    HostLogic &operator=(HostLogic &&) = delete;

    friend void host_signal_handler(int, siginfo_t *, void *);
    static HostLogic &get_instance(ConnectionType type, int host_pid, int client_pid)
    {
        static HostLogic instance(type, host_pid, client_pid);
        return instance;
    }
    bool send_to_client(const std::string &message)
    {
        bool f = host_to_client->Write(message);
        kill(client_pid, SIGUSR1);
        return f;
    }

    bool read_from_client(std::string &message)
    {
        return client_to_host->Read(message);
    }

    ~HostLogic()
    {
        kill(client_pid, SIGKILL);
    }
};