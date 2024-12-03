#pragma once

#include "../connections_/connection_fabric.hpp"

void host_signal_handler(int, siginfo_t *, void *);

class HostLogic {
private:
    int host_pid;
    std::unordered_map<int, std::unique_ptr<Connection>> client_connections;
    struct sigaction signal_handler;

    HostLogic(ConnectionType type, int host_pid) : host_pid(host_pid) {
        signal_handler.sa_sigaction = host_signal_handler;
        signal_handler.sa_flags = SA_SIGINFO;
        if (sigaction(SIGUSR1, &signal_handler, nullptr) == -1) {
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
        auto host_to_client = ConnectionFabric::create_connection(type, ConnectionFabric::create_filename(type, host_pid, client_pid), true);
        client_connections.emplace(client_pid, std::move(host_to_client));
    }

    bool send_to_client(int client_pid, const std::string& msg) {
        if (client_connections.count(client_pid)) {
            client_connections[client_pid]->Write(msg);
            kill(client_pid, SIGUSR1);
            return true;
        }
        return false;
    }

    bool read_from_client(int& client_pid, std::string& msg) {
        for (auto& [pid, conn] : client_connections) {
            if (conn->Read(msg)) {
                client_pid = pid;
                return true;
            }
        }
        return false;
    }
};
