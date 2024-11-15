#pragma once

#include "../includes/includes.hpp"
#include "../connections/all_connections.hpp"

template <Conn T>
class ClientInfo
{
private: 
    int host_pid;
    int pid;
    // T host_to_client;
    // T client_to_host;
    // T host_to_client_general;
    // T client_to_host_general;
    std::vector<T> connections;
    // std::jthread thread;
    // std::chrono::time_point<std::chrono::steady_clock> time_point;
    // void run();
public:
    ClientInfo(int host_pid, int pid, bool create = true) : host_pid(host_pid), pid(pid)
    {
        connections.emplace_back(T::make_filename(host_pid, pid), create);
        connections.emplace_back(T::make_filename(pid, host_pid), create);
        connections.emplace_back(T::make_general_filename(host_pid, pid), create);
        connections.emplace_back(T::make_general_filename(pid, host_pid), create);
        sem_t* semaphore = sem_open((T::make_filename(host_pid, pid) + "_creation").c_str(), O_CREAT, 0777, 0);
        if (semaphore == SEM_FAILED)
            throw std::runtime_error("Error to create a semaphore!");
        sem_post(semaphore);
    }
    ClientInfo() = default;
    ClientInfo(const ClientInfo &) = default;
    ClientInfo &operator=(const ClientInfo &) = default;
    ClientInfo(ClientInfo &&) = default;
    ClientInfo &operator=(ClientInfo &&) = default;

    bool send_to_client(const std::string &message)
    {
        bool f = connections[0].Write(message);
        kill(host_pid, SIGUSR1);
        return f;
    }

    bool read_from_client(std::string &message)
    {
        return connections[1].Read(message);
    }

    bool send_to_client_general(const std::string &message)
    {
        bool f = connections[2].Write(message);
        kill(host_pid, SIGUSR2);
        return f;
    }

    bool read_from_client_general(std::string &message)
    {
        return connections[3].Read(message);
    }
    // void start();
};