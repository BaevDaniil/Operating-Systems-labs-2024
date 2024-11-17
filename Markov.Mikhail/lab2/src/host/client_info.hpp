#pragma once

#include "../includes/includes.hpp"
#include "../connections/all_connections.hpp"

template <Conn T>
class ClientInfo
{
private: 
    int host_pid;
    int pid;
    mutable std::shared_ptr<std::mutex> m_queue;
    mutable std::shared_ptr<std::mutex> general_m_queue;
    // T host_to_client;
    // T client_to_host;
    // T host_to_client_general;
    // T client_to_host_general;
    std::vector<T> connections;

    std::queue<std::string> unwritten_messages;
    std::queue<std::string> general_unwritten_messages;
    std::chrono::time_point<std::chrono::steady_clock> time_point;
    std::shared_ptr<std::jthread> worker;
    bool pop_unwritten_message(std::string &msg)
    {
        std::lock_guard<std::mutex> m_lock(*m_queue);
        if (unwritten_messages.empty())
            return false;
        while (!unwritten_messages.empty())
        {
            msg += unwritten_messages.front();
            unwritten_messages.pop();
        }
        return true;
    }

    bool pop_general_unwritten_message(std::string &msg)
    {
        std::lock_guard<std::mutex> m_lock(*general_m_queue);
        if (general_unwritten_messages.empty())
            return false;
        msg += general_unwritten_messages.front();
        general_unwritten_messages.pop();
        return true;
    }

public:
    ClientInfo(int host_pid, int pid, bool create = true) : host_pid(host_pid), pid(pid), m_queue(std::make_shared<std::mutex>()),
                                                            general_m_queue(std::make_shared<std::mutex>())
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
        update_time();
        bool f = connections[0].Write(message);
        kill(pid, SIGUSR1);
        return f;
    }

    bool read_from_client(std::string &message)
    {
        update_time();
        return connections[1].Read(message);
    }

    bool send_to_client_general(const std::string &message)
    {
        update_time();
        bool f = connections[2].Write(message);
        kill(pid, SIGUSR2);
        return f;
    }

    bool read_from_client_general(std::string &message)
    {
        update_time();
        return connections[3].Read(message);
    }

    void push_message(const std::string& msg)
    {
        std::lock_guard<std::mutex> m_lock(*m_queue);
        unwritten_messages.push(msg);
    }

    void push_general_message(const std::string &msg)
    {
        std::lock_guard<std::mutex> m_lock(*general_m_queue);
        general_unwritten_messages.push(msg);
    }
    void start()
    {   
        using namespace std::chrono_literals;
        auto func = [this]()
        {
            std::string msg;
            bool f1;
            bool f2;
            while(true)
            {
                f1 = pop_unwritten_message(msg);
                if (f1)
                {
                    std::cout << "send_to_client" << std::endl;
                    send_to_client(msg);
                    msg.clear();
                }
                f2 = pop_general_unwritten_message(msg);
                if (f2)
                {
                    std::cout << "send_to_client_general" << std::endl;
                    send_to_client_general(msg);
                    msg.clear();
                }
                if (f1 || f2)
                {
                    if (std::chrono::steady_clock::now() - time_point > std::chrono::minutes(1))
                    {
                        return true;
                    }
                }
                std::this_thread::sleep_for(100ms);
            }
            return false;
        };
        worker = std::make_shared<std::jthread>(std::jthread(std::move(func)));
    }

    void update_time()
    {
        time_point = std::chrono::steady_clock::now();
    }
};