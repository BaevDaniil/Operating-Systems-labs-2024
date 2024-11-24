#pragma once

#include "../includes/includes.hpp"
#include "../connections/all_connections.hpp"

void client_signal_handler(int, siginfo_t*, void*);
void processIncomingRequests(int signal, const std::string& request);

template <Conn ConnectionType>
class ClientHandler
{
    friend class ClientWindow;

    int server_pid{};
    int client_pid{};
    std::vector<ConnectionType> connection_channels{};
    struct sigaction signal_handler{};
    friend void client_signal_handler(int, siginfo_t*, void*);

    ClientHandler(const std::string& pid_file_path, bool initialize)
    {
        std::fstream pid_file(pid_file_path);
        if (!pid_file)
        {
            throw std::runtime_error(
                "Configuration file not found: " + std::filesystem::absolute(pid_file_path).string());
        }

        pid_file >> server_pid;
        pid_file.close();
        std::cerr << "Server PID: " << server_pid << std::endl;

        client_pid = getpid();
        std::cerr << "Client PID: " << client_pid << std::endl;

        signal_handler.sa_sigaction = client_signal_handler;
        signal_handler.sa_flags = SA_SIGINFO;

        if (sigaction(SIGUSR1, &signal_handler, nullptr) == -1 ||
            sigaction(SIGUSR2, &signal_handler, nullptr) == -1)
        {
            throw std::runtime_error("Failed to register signal handlers.");
        }

        kill(server_pid, SIGUSR1);

        sem_t* semaphore = sem_open(
            (ConnectionType::make_filename(server_pid, client_pid) + "_init").c_str(),
            O_CREAT, 0777, 0);

        timespec timeout{};
        if (clock_gettime(CLOCK_REALTIME, &timeout) == -1)
        {
            throw std::runtime_error("Failed to retrieve current time.");
        }

        timeout.tv_sec += 5;

        int sem_wait_result;
        while ((sem_wait_result = sem_timedwait(semaphore, &timeout)) == -1 && errno == EINTR)
        {
            continue;
        }

        if (sem_wait_result == -1)
        {
            throw std::runtime_error("Semaphore wait timed out.");
        }


        connection_channels.emplace_back(ConnectionType::make_filename(server_pid, client_pid) + "r", initialize);
        connection_channels.emplace_back(ConnectionType::make_filename(client_pid, server_pid) + "r", initialize);
        connection_channels.emplace_back(ConnectionType::make_filename(server_pid, client_pid) + "ans", initialize);
        connection_channels.emplace_back(ConnectionType::make_filename(client_pid, server_pid) + "ans", initialize);

        sem_unlink((ConnectionType::make_filename(server_pid, client_pid) + "_init").c_str());
    }

public:
    sem_t* client_semaphore{};
    std::string semaphore_name{};

    static ClientHandler& getInstance(const std::string& pid_file_path, bool initialize = false)
    {
        static ClientHandler instance(pid_file_path, initialize);
        return instance;
    }

    bool sendRequest(int signal, const std::string& request)
    {
        int channel_index = (signal == SIGUSR1) ? 1 : 3;
        std::cerr << "Sending request on channel " << channel_index << ": " << request << std::endl;

        bool result = connection_channels[channel_index].Write(request);
        kill(server_pid, signal);
        return result;
    }

    bool readResponse(std::string& response, bool is_return_answer = true)
    {
        int channel_index = is_return_answer ? 0 : 2;
        return connection_channels[channel_index].Read(response);
    }

    ~ClientHandler()
    {
        sem_close(client_semaphore);
    }
};

namespace client_config
{
    constexpr bool identifier = false;
    const std::filesystem::path pid_file_path = std::filesystem::current_path() / "host.pid";
}