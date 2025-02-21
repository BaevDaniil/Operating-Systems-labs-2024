#pragma once

#include "../connections/connection_fabric.hpp"

void client_signal_handler(int, siginfo_t *, void *);

class ClientLogic
{
private:
    int host_pid;
    int pid;
    std::unique_ptr<Connection> host_to_client;
    std::unique_ptr<Connection> client_to_host;
    struct sigaction signal_handler;
    ClientLogic(ConnectionType type, int host_pid, int client_pid) : host_pid(host_pid), pid(client_pid)
    {
        // std::cout << "host_pid = " << host_pid << std::endl;
        // std::cout << "client_pid = " << pid << std::endl;
        signal_handler.sa_sigaction = client_signal_handler;
        signal_handler.sa_flags = SA_SIGINFO;

        if (sigaction(SIGUSR1, &signal_handler, nullptr) == -1)
            throw std::runtime_error("Failed to register signal handler");

        sem_t *semaphore = sem_open(("sem" + ConnectionFabric::create_filename(type, host_pid, pid)).c_str(), O_CREAT, 0777, 0);
        struct timespec tm;
        int s;
        if (clock_gettime(CLOCK_REALTIME, &tm) == -1)
            throw std::runtime_error("Failed to get real time");
        tm.tv_sec += 5;
        while ((s = sem_timedwait(semaphore, &tm)) == -1 && errno == EINTR)
            continue;

        host_to_client = std::move(ConnectionFabric::create_connection(type, ConnectionFabric::create_filename(type, host_pid, pid), false));
        client_to_host = std::move(ConnectionFabric::create_connection(type, ConnectionFabric::create_filename(type, pid, host_pid), false));

        sem_unlink(("sem" + ConnectionFabric::create_filename(type, host_pid, pid)).c_str());
    }

public:
    ClientLogic(const ClientLogic &) = delete;
    ClientLogic &operator=(const ClientLogic &) = delete;
    ClientLogic(ClientLogic &&) = delete;
    ClientLogic &operator=(ClientLogic &&) = delete;

    friend void client_signal_handler(int, siginfo_t *, void *);
    static ClientLogic &get_instance(ConnectionType type, int host_pid, int client_pid)
    {
        static ClientLogic instance(type, host_pid, client_pid);
        return instance;
    }
    bool send_to_host(const std::string& message)
    {;
        bool f = client_to_host->Write(message);
        kill(host_pid, SIGUSR1);
        return f;
    }

    bool read_from_host(std::string& message)
    {
        return host_to_client->Read(message);
    }

    ~ClientLogic() = default;
};