#pragma once

#include "../connections_/connection_fabric.hpp"

void host_signal_handler(int, siginfo_t *, void *);

class HostLogic {
private:
    std::ofstream log_file;
    std::mutex log_mutex;

    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);
        log_file << "[Host] " << message << std::endl;
    }

    int host_pid;
    std::unordered_map<int, std::unique_ptr<Connection>> general_host_to_client_connection;
    std::unordered_map<int, std::unique_ptr<Connection>> general_client_to_host_connection;
    std::unordered_map<int, std::unique_ptr<Connection>> host_to_client_connection;
    std::unordered_map<int, std::unique_ptr<Connection>> client_to_host_connection;
    struct sigaction signal_handler;

    HostLogic(ConnectionType type, int host_pid) : log_file("host_log.txt"), host_pid(host_pid) {
        log("Host initialized with PID " + std::to_string(host_pid));
        signal_handler.sa_sigaction = host_signal_handler;
        signal_handler.sa_flags = SA_SIGINFO;
        if (sigaction(SIGUSR1, &signal_handler, nullptr) == -1 || 
            sigaction(SIGUSR2, &signal_handler, nullptr) == -1) {
            throw std::runtime_error("Failed to register signal handler");
        }
    }

public:
    friend void host_signal_handler(int, siginfo_t *, void *);
    static HostLogic& get_instance(ConnectionType type, int host_pid) {
        static HostLogic instance(type, host_pid);
        return instance;
    }

    void add_client(int client_pid, ConnectionType type) {
        host_to_client_connection.emplace(client_pid, std::move(ConnectionFabric::create_connection(type, ConnectionFabric::create_filename(type, host_pid, client_pid), true)));
        client_to_host_connection.emplace(client_pid, std::move(ConnectionFabric::create_connection(type, ConnectionFabric::create_filename(type, client_pid, host_pid), true)));
        general_host_to_client_connection.emplace(client_pid, std::move(ConnectionFabric::create_connection(type, ConnectionFabric::create_general_filename(type, host_pid, client_pid), true)));
        general_client_to_host_connection.emplace(client_pid, std::move(ConnectionFabric::create_connection(type, ConnectionFabric::create_general_filename(type, client_pid, host_pid), true)));
    }

    bool send_to_client_general(int client_pid, const std::string& msg) {
        
        if (general_host_to_client_connection.count(client_pid)) {
            general_host_to_client_connection[client_pid]->Write(msg);
            log("Sent message to client " + std::to_string(client_pid) + " in general chat: " + msg);
            kill(client_pid, SIGUSR2);
            return true;
        }
        return false;
    }

    bool send_to_client_private(int client_pid, const std::string& msg) {
        
        if (host_to_client_connection.count(client_pid)) {
            log("Sent message to client " + std::to_string(client_pid) + " in private chat: " + msg);
            host_to_client_connection[client_pid]->Write("Host: " + msg);
            kill(client_pid, SIGUSR1);
            return true;
        }
        return false;
    }

    bool read_from_client_general(int& client_pid, std::string& msg) {
        auto& [pid, conn] = *general_client_to_host_connection.find(client_pid);
        if (conn->Read(msg)) {
            log("Host recived general msg from "+ std::to_string(client_pid) + " " + msg);
            return true;
        }
        return false;
    }

    bool read_from_client_private(int& client_pid, std::string& msg) {
        auto& [pid, conn] = *client_to_host_connection.find(client_pid);
        log("Try to read private message");
        if (conn->Read(msg)) {
            log("Host recived private msg from "+ std::to_string(client_pid) + " " + msg);
            return true;
        }
        log("Empty reading for PID: " + std::to_string(client_pid));
        return false;
    }
};
