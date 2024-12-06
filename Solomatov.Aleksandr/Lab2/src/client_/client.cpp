#include "client.hpp"

Client::Client(ConnectionType type, pid_t host_pid, pid_t client_pid, sighendlerType sig_hendler): 
conlogic(type, host_pid, client_pid, sig_hendler), active(true), idle(false) 
{
        idleTimerThread = std::thread(&Client::handleTimeout, this);
}

void Client::process_host_chat(){   
    std::string msg;
    conlogic.read_from_host(msg);
    window->set_message_on_chat(msg);
}

void Client::process_general_chat(){
        std::string msg;
        conlogic.read_from_host_general(msg);
        window->setMessageToGeneralChat(msg);
}

void Client::send_to_host_general(const std::string& msg){
        conlogic.send_to_host_general(msg);
}


void Client::send_to_host(const std::string &msg)
{
        conlogic.send_to_host(msg);
}


void Client::handleTimeout() {
    while (active) {
        std::this_thread::sleep_for(std::chrono::seconds(idleTimeoutSeconds));
        if (idle) {
            std::cerr << "Idle timeout reached. Exiting..." << std::endl;
            active = false;
            std::exit(0);
        }
        idle = true;
    }
}

void Client::resetIdleTimer() {
    idle = false; // сброс таймера активности
}

Client::~Client() {
    active = false;
    conlogic.~ClientConnectionLogic();
    if (listenThread.joinable()) listenThread.join();
    if (idleTimerThread.joinable()) idleTimerThread.join();
}
