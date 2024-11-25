#pragma once

#include "../includes/includes.hpp"
#include "../connections/all_connections.hpp"

template <Conn T>
class ClientInfo
{
    int host_pid;
    int pid;
    std::vector<T> connections;

public:
    ClientInfo(int host_pid, int pid, bool create = true) : host_pid(host_pid), pid(pid)
    {
        connections.emplace_back(T::make_filename(host_pid, pid), create); // send return answer to client
        connections.emplace_back(T::make_filename(pid, host_pid), create); // read return request from client
        connections.emplace_back(T::make_filename(host_pid, pid), create); // send take answer to client
        connections.emplace_back(T::make_filename(pid, host_pid), create); // read take request from client
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
    ~ClientInfo() = default;

    bool read_take_request_from_client(std::string &request)
    {
        return connections[3].Read(request);
    }

    bool read_return_request_from_client(std::string &request)
    {
        return connections[1].Read(request);
    }

    bool send_return_answer_to_client(const std::string &answer)
    {
        bool f = connections[0].Write(answer);
        kill(pid, SIGUSR1);
        std::cerr<<"send signal 1"<<std::endl;
        return f;
    }

    bool send_take_answer_to_client(const std::string &answer)
    {
        bool f = connections[2].Write(answer);
        kill(pid, SIGUSR2);
        std::cerr<<"send signal 2"<<std::endl;
        return f;
    }
};
