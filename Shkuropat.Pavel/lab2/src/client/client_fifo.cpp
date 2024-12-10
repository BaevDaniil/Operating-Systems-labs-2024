#include "conn_fifo.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

int main() {
    try {
        ConnFifo conn(1, false); // Подключаемся как клиент
        std::cout << "[FIFO Client] Initialized and running..." << std::endl;

        while (true) {
            std::string input;
            std::cout << "[FIFO Client] Enter message (type 'exit' to quit): ";
            std::getline(std::cin, input);

            if (input == "exit") {
                conn.Write(input.c_str(), input.size() + 1);
                break;
            }

            conn.Write(input.c_str(), input.size() + 1);

            char buffer[1024] = {0};
            conn.Read(buffer, sizeof(buffer));
            std::cout << "[FIFO Client] Response: " << buffer << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "[FIFO Client] Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "[FIFO Client] Shutting down..." << std::endl;
    return 0;
}
