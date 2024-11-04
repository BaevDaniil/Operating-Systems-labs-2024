#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "conn_socket.hpp"  // Включаем заголовочный файл для работы с сокетами

const int PORT = 12345;  // Порт для прослушивания входящих соединений

void handleClient(ConnSocket clientSocket) {
    if (!clientSocket.IsValid()) {
        std::cerr << "Ошибка: невалидный дескриптор сокета при подключении клиента\n";
        return;
    }

    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = clientSocket.Read(buffer, sizeof(buffer));
        if (bytesReceived < 0) {
            std::cerr << "Ошибка получения данных: " << strerror(errno) << "\n";
            break;
        } else if (bytesReceived == 0) {
            std::cout << "Клиент отключился\n";
            break;
        }
        std::cout << "Получено сообщение от клиента: " << buffer << std::endl;

        // Echo back to client
        if (!clientSocket.Write(buffer, bytesReceived)) {
            std::cerr << "Ошибка отправки данных клиенту\n";
            break;
        }
    }

    clientSocket.Close();
}

int main() {
    ConnSocket serverSocket;

    if (!serverSocket.CreateServerSocket(PORT)) {
        std::cerr << "Ошибка при создании серверного сокета\n";
        return 1;
    }

    std::cout << "Сервер запущен и прослушивает порт " << PORT << std::endl;

    while (true) {
        ConnSocket clientSocket = serverSocket.AcceptConnection();
        if (!clientSocket.IsValid()) {
            std::cerr << "Ошибка принятия соединения\n";
            continue;
        }

        std::cout << "Новое подключение клиента\n";
        handleClient(std::move(clientSocket));
    }

    return 0;
}
