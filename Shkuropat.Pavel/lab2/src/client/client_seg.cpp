#include "conn_seg.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

int main() {
    try {
        ConnSeg conn(1, false); // Подключаемся как клиент
        std::cout << "[SHM Client] Initialized and running..." << std::endl;

        while (true) {
            std::string input;
            std::cout << "[SHM Client] Enter message (type 'exit' to quit): ";
            std::getline(std::cin, input);

            if (input == "exit") {
                conn.Write(input.c_str(), input.size() + 1);
                break;
            }

            conn.Write(input.c_str(), input.size() + 1);

            char buffer[1024] = {0};
            conn.Read(buffer, sizeof(buffer));
            std::cout << "[SHM Client] Response: " << buffer << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "[SHM Client] Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "[SHM Client] Shutting down..." << std::endl;
    return 0;
}
