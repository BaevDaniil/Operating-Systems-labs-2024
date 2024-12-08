#include "conn_mq.hpp"
#include <iostream>
#include <thread>
#include <cstring>
#include <chrono>

int main() {
    try {
        ConnMq conn(1, true); // Инициализация очереди сообщений как хоста
        std::cout << "[MQ Host] Initialized and running..." << std::endl;

        while (true) {
            char buffer[1024] = {0};

            // Чтение сообщений из очереди сообщений
            conn.Read(buffer, sizeof(buffer));
            if (strlen(buffer) > 0) {
                std::cout << "[MQ Host] Received: " << buffer << std::endl;

                // Если получено сообщение "exit", завершаем программу
                if (std::string(buffer) == "exit") {
                    std::cout << "[MQ Host] Client requested termination." << std::endl;
                    break;
                }

                // Эхо-ответ клиенту
                std::string response = "Echo from MQ Host: " + std::string(buffer);
                conn.Write(response.c_str(), response.size() + 1);
            }

            // Небольшая пауза
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const std::exception &e) {
        std::cerr << "[MQ Host] Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "[MQ Host] Shutting down..." << std::endl;
    return 0;
}
