#include "conn_fifo.hpp"
#include <iostream>
#include <thread>
#include <cstring>
#include <chrono>

int main() {
    try {
        ConnFifo conn(1, true); // Инициализация FIFO как хоста
        std::cout << "[FIFO Host] Initialized and running..." << std::endl;

        while (true) {
            char buffer[1024] = {0};

            // Чтение сообщений из FIFO
            conn.Read(buffer, sizeof(buffer));
            if (strlen(buffer) > 0) {
                std::cout << "[FIFO Host] Received: " << buffer << std::endl;

                // Если получено сообщение "exit", завершаем программу
                if (std::string(buffer) == "exit") {
                    std::cout << "[FIFO Host] Client requested termination." << std::endl;
                    break;
                }

                // Эхо-ответ клиенту
                std::string response = "Echo from FIFO Host: " + std::string(buffer);
                conn.Write(response.c_str(), response.size() + 1);
            }

            // Небольшая пауза
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const std::exception &e) {
        std::cerr << "[FIFO Host] Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "[FIFO Host] Shutting down..." << std::endl;
    return 0;
}
