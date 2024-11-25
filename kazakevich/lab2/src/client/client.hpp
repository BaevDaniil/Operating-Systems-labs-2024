#pragma once

#include "../includes/includes.hpp"
#include "../connections/all_connections.hpp"

void client_signal_handler(int, siginfo_t *, void *);
void processRequests(int sig, std::string request);

template <Conn T>
class Client
{
    friend class ClientWindow;
    int host_pid{};
    int pid;
    std::vector<T> connections;
    struct sigaction signal_handler{};
    friend void client_signal_handler(int, siginfo_t *, void *);


    Client(const std::string &host_pid_path, bool create)
    {
        std::fstream file(host_pid_path);
        if (!file)
            throw std::runtime_error("No config file" + std::string(std::filesystem::absolute(host_pid_path)));
        file >> host_pid;
        file.close();
        std::cerr<<host_pid<<std::endl;
        pid = getpid();
        std::cerr<<pid<<std::endl;
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
        timespec tm{};
        int s;
        if (clock_gettime(CLOCK_REALTIME, &tm) == -1)
            throw std::runtime_error("Failed to get real time");
        tm.tv_sec += 5;
        while ((s = sem_timedwait(semaphore, &tm)) == -1 && errno == EINTR)
            continue;

        connections.emplace_back(T::make_filename(host_pid, pid), create);  // read return answer from host
        connections.emplace_back(T::make_filename(pid, host_pid), create);  // send return request to host
        connections.emplace_back(T::make_filename(host_pid, pid), create);  // read take answer from host
        connections.emplace_back(T::make_filename(pid, host_pid), create);  // send take request to host

        sem_unlink((T::make_filename(host_pid, pid) + "_creation").c_str());
    }



public:
    sem_t* semaphore{};
    std::string sem_name;

    static Client& get_instance(const std::string &host_pid_path, bool create = false)
    {
        static Client instance(host_pid_path, create);
        return instance;
    }
    bool send_request_to_host(int signal, const std::string& request)
    {
        int id = (signal == SIGUSR1) ? 1 : 3;
        std::cerr<<id<<std::endl;
        std::cerr<<request<<std::endl;
        bool f = connections[id].Write(request);
        kill(host_pid, signal);
        return f;
    }

    bool read_return_answer_from_host(std::string& answer)
    {
        return connections[0].Read(answer);
    }

    bool read_take_answer_from_host(std::string& answer)
    {
        return connections[2].Read(answer);
    }


    ~Client() {
        sem_close(semaphore);
    }
};

namespace client_namespace
{
    const bool identifier = false;
    const std::filesystem::path host_pid_path = std::filesystem::current_path() / "host.pid";
}