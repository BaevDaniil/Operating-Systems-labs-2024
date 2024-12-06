#pragma once

#include "../connections_/connection_fabric.hpp"

using sighendlerType = void(*)(int, siginfo_t *, void *);
// void client_signal_handler(int, siginfo_t *, void *);

class ClientConnectionLogic
{
private:
    std::ofstream log_file; // Файл логов клиента
    std::mutex log_mutex;   // Мьютекс для потокобезопасности

    int host_pid;
    int pid;
    std::unique_ptr<Connection> general_host_to_client_connection;
    std::unique_ptr<Connection> general_client_to_host_connection;
    std::unique_ptr<Connection> host_to_client_connection;
    std::unique_ptr<Connection> client_to_host_connection;
    struct sigaction signal_handler;

    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);
        log_file << "[Client " << pid << "] " << message << std::endl;
    }
public:
    ClientConnectionLogic(const ClientConnectionLogic &) = delete;
    ClientConnectionLogic &operator=(const ClientConnectionLogic &) = delete;
    ClientConnectionLogic(ClientConnectionLogic &&) = delete;
    ClientConnectionLogic &operator=(ClientConnectionLogic &&) = delete;
    ~ClientConnectionLogic() = default;

    ClientConnectionLogic(ConnectionType type, pid_t host_pid, pid_t pid, sighendlerType client_sig_handler):
    log_file("client_" + std::to_string(pid) + "_log.txt"), host_pid(host_pid), pid(pid)
    {
        log("Client initialized with PID " + std::to_string(pid));
        this->pid = pid; 
        signal_handler.sa_sigaction = client_sig_handler;
        signal_handler.sa_flags = SA_SIGINFO;
        
        if (sigaction(SIGUSR1, &signal_handler, nullptr) == -1 || 
            sigaction(SIGUSR2, &signal_handler, nullptr) == -1)
            throw std::runtime_error("Failed to register signal handler");

        host_to_client_connection = std::move(ConnectionFabric::create_connection(type, ConnectionFabric::create_filename(type, host_pid, pid), false));
        client_to_host_connection = std::move(ConnectionFabric::create_connection(type, ConnectionFabric::create_filename(type, pid, host_pid), false));
        general_host_to_client_connection = std::move(ConnectionFabric::create_connection(type, ConnectionFabric::create_general_filename(type, host_pid, pid), false));
        general_client_to_host_connection = std::move(ConnectionFabric::create_connection(type, ConnectionFabric::create_general_filename(type, pid, host_pid), false));

    }

    bool send_to_host(const std::string& message)
    {
        log("Sending message to host private chat: " + message);
        bool f = client_to_host_connection->Write(message);
        kill(host_pid, SIGUSR1);
        return f;
    }

    bool read_from_host(std::string& message)
    {
        return host_to_client_connection->Read(message);
    }

    bool send_to_host_general(const std::string &message)
    {
        log("Sending message to host general chat: " + message);
        bool f = general_client_to_host_connection->Write(message);
        kill(host_pid, SIGUSR2);
        return f;
    }

    bool read_from_host_general(std::string &message)
    {
        bool f = general_host_to_client_connection->Read(message);
        if (f) {
            log("Received message from host general chat: " + message);
        } else {
            log("No message received from host general chat.");
        }
        return f;
    }
};