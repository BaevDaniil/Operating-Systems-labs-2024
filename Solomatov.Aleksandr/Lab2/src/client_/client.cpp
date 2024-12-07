#include "client.hpp"

Client::Client(ConnectionType type, pid_t host_pid, pid_t client_pid, sighendlerType sig_hendler): conlogic(std::make_unique<ClientConnectionLogic>(type, host_pid, client_pid, sig_hendler)), 
    active(true), idle(false)
{
    idleTimerThread = std::thread(&Client::handleTimeout, this);
}

void Client::process_host_chat(){   
    std::string msg;
    conlogic->read_from_host(msg);
    window->set_message_on_chat(msg);
}

void Client::process_general_chat(){
        std::string msg;
        conlogic->read_from_host_general(msg);
        window->setMessageToGeneralChat(msg);
}

void Client::handleTimeout() {
    while (active.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(idleTimeoutSeconds));
        if (idle.load()) {
            if (active.load()){
                std::cerr << "Idle timeout reached. Exiting..." << std::endl;
                conlogic.reset();
                std::exit(0);
            }
            break;
        }
        idle = true;
    }
}

void Client::resetIdleTimer() {
    idle = false;
}

Client::~Client() {
    idle = true;
    active = false;
    if (idleTimerThread.joinable()) idleTimerThread.join();
}
