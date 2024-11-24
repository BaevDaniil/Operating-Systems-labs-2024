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
    std::shared_ptr<std::atomic<int>> unread_messages;
    // T host_to_client;
    // T client_to_host;
    // T host_to_client_general;
    // T client_to_host_general;
    std::vector<T> connections;

    std::queue<std::string> unwritten_messages;
    std::queue<std::string> general_unwritten_messages;
    std::chrono::time_point<std::chrono::steady_clock> time_point;
    std::shared_ptr<std::jthread> worker;
    bool valid = true;
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

    bool send_to_client(const std::string &message)
    {
        bool f = connections[0].Write(message);
        kill(pid, SIGUSR1);
        return f;
    }

    bool send_to_client_general(const std::string &message)
    {
        bool f = connections[2].Write(message);
        kill(pid, SIGUSR2);
        return f;
    }

public:
    ClientInfo(int host_pid, int pid, bool create = true) : host_pid(host_pid), pid(pid),
        m_queue(std::make_shared<std::mutex>()), general_m_queue(std::make_shared<std::mutex>()),
        unread_messages(std::make_shared<std::atomic<int>>())
    {
        connections.emplace_back(T::make_filename(host_pid, pid), create);
        connections.emplace_back(T::make_filename(pid, host_pid), create);
        connections.emplace_back(T::make_general_filename(host_pid, pid), create);
        connections.emplace_back(T::make_general_filename(pid, host_pid), create);
        sem_t* semaphore = sem_open((T::make_filename(host_pid, pid) + "_creation").c_str(), O_CREAT, 0777, 0);
        if (semaphore == SEM_FAILED)
            throw std::runtime_error("Error to create a semaphore!");
        sem_post(semaphore);
        update_time();
    }
    ClientInfo() = default;
    ClientInfo(const ClientInfo &) = default;
    ClientInfo &operator=(const ClientInfo &) = default;
    ClientInfo(ClientInfo &&) = default;
    ClientInfo &operator=(ClientInfo &&) = default;
    ~ClientInfo() = default;

    bool is_valid()
    {
        return valid;
    }

    bool read_from_client(std::string &message)
    {
        update_time();
        return connections[1].Read(message);
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
    void start(auto&& mainwindow_pointer, auto&& f)
    {   
        using namespace std::chrono_literals;
        auto func = [this, &f, &mainwindow_pointer]()
        {
            std::string msg;
            bool f1;
            bool f2;
            while(valid)
            {
                while(*unread_messages > 0)
                {
                    read_from_client(msg);
                    --(*unread_messages);
                    std::invoke(f, mainwindow_pointer, pid, msg);
                    msg.clear();
                }
                f1 = pop_unwritten_message(msg);
                if (f1)
                {
                    send_to_client(msg);
                    msg.clear();
                }
                f2 = pop_general_unwritten_message(msg);
                if (f2)
                { 
                    send_to_client_general(msg);
                    msg.clear();
                }
                if (std::chrono::steady_clock::now() - time_point > std::chrono::seconds(60))
                {
                    kill(pid, SIGKILL);
                    valid = false;
                    return true;
                }
                std::this_thread::sleep_for(10ms);
            }
            return false;
        };
        worker = std::make_shared<std::jthread>(std::jthread(std::move(func)));
    }

    void update_time()
    {
        time_point = std::chrono::steady_clock::now();
    }

    void stop()
    {
        valid = false;
        if (worker && (*worker).joinable())
            (*worker).join();
    }
    void append_unread_counter()
    {
        ++(*unread_messages);
    }
};
