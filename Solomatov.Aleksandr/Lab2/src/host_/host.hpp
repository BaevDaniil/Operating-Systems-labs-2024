#pragma once

#include "host_logic.hpp"
#include "../gui/chat_gui.hpp"



class Host {
private:
    HostLogic& host_logic_ref;
    MainWindow* window;
    std::unordered_map<int, std::chrono::time_point<std::chrono::steady_clock>> client_activity;
    std::atomic<bool> running;
    std::mutex activity_mutex;

    Host(HostLogic& ref) : host_logic_ref(ref), running(true) {
        std::thread(&Host::check_inactive_clients, this).detach();
    }

    void check_inactive_clients() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(60));
            std::lock_guard<std::mutex> lock(activity_mutex);
            auto now = std::chrono::steady_clock::now();
            for (auto it = client_activity.begin(); it != client_activity.end();) {
                if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count() > 60) {
                    kill(it->first, SIGKILL);
                    window->remove_client(it->first);
                    it = client_activity.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

public:
    static Host& get_instance(ConnectionType type, int host_pid) {
        HostLogic& host_logic = HostLogic::get_instance(type, host_pid);
        static Host instance(host_logic);
        return instance;
    }

    void start_inactivity_check() {
        std::thread(&Host::check_inactive_clients, this).detach();
    }


    void set_up_ui(MainWindow* ptr) {
        if (ptr) window = ptr;
    }
    

    void process_general_chat(int client_pid) {
        std::string msg;
        bool received = host_logic_ref.read_from_client_general(client_pid, msg);
        if (received) {
            std::lock_guard<std::mutex> lock(activity_mutex);
            client_activity[client_pid] = std::chrono::steady_clock::now();
            msg = "Client [" + std::to_string(client_pid) + "]: " + msg;
            window->set_msg_to_general_chat(msg);                
            resend_to_clients_except_one(msg, client_pid);
        }
    }

    void process_private_chat(int client_pid) {
        std::string msg;
        bool received = host_logic_ref.read_from_client_private(client_pid, msg);
        if (received) {
            std::lock_guard<std::mutex> lock(activity_mutex);
            client_activity[client_pid] = std::chrono::steady_clock::now();
            msg = "Client private: " + msg;
            window->set_msg_to_private_chat(client_pid,msg);
        }
    }      

    void send_to_client_general(int client_pid, const std::string& msg) {
        if (!host_logic_ref.send_to_client_general(client_pid, msg)) {
            window->remove_client(client_pid);
        }
    }

    void send_to_client_private(int client_pid, const std::string& msg) {
        if (!host_logic_ref.send_to_client_private(client_pid, msg)) {
            window->remove_client(client_pid);
        }
    }

    void send_to_all_clients(const std::string& msg) {
        for (const auto& [pid, _] : client_activity) {
            send_to_client_general(pid,"Host: " + msg);
        }
    }

    void resend_to_clients_except_one(const std::string& msg, pid_t client_sender){
        for (const auto& [pid, _] : client_activity) {
            if (pid != client_sender) send_to_client_general(pid, msg);
        }
    }

    void add_client(int client_pid, ConnectionType type) {
        {
            std::lock_guard<std::mutex> lock(activity_mutex);
            client_activity[client_pid] = std::chrono::steady_clock::now();
        }
        host_logic_ref.add_client(client_pid, type);
        if (window) {
            QMetaObject::invokeMethod(window, "add_client", Q_ARG(int, client_pid));
        }
    }

    ~Host() {
        running = false;
    }
};
