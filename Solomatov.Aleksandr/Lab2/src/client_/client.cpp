#include "client.hpp"

Client::Client(const std::string &connectionId, bool create, std::unique_ptr<Connection> conn)
    : connection(std::move(conn)), active(true), idle(false) {

    // Запуск потока для слежения за таймером
    idleTimerThread = std::thread(&Client::handleTimeout, this);
}

void Client::start() {
    listenThread = std::thread(&Client::listen, this);
}

void Client::sendMessage(const std::string &message) {
    if (connection && active) {
        if (connection->Write(message)) {
            std::cout << "Sent: " << message << std::endl;
            resetIdleTimer();
        } else {
            std::cerr << "Failed to send message." << std::endl;
        }
    }
}

void Client::listen() {
    std::string message;
    while (active) {
        if (connection->Read(message)) {
            resetIdleTimer();
            std::cout << "Received: " << message << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // минимальная пауза для снижения нагрузки
    }
}

void Client::handleTimeout() {
    while (active) {
        std::this_thread::sleep_for(std::chrono::seconds(idleTimeoutSeconds));
        if (idle) {
            std::cerr << "Idle timeout reached. Exiting..." << std::endl;
            active = false;
            std::exit(0);
        }
        idle = true; // помечаем как "неактивный", если не было взаимодействия
    }
}

void Client::resetIdleTimer() {
    idle = false; // сброс таймера активности
}

Client::~Client() {
    active = false;
    if (listenThread.joinable()) listenThread.join();
    if (idleTimerThread.joinable()) idleTimerThread.join();
}
