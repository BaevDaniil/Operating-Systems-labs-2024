#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <unistd.h>
#include "conn_socket.hpp"

const std::string SERVER_ADDRESS = "127.0.0.1";  // Локальный адрес сервера
const int SERVER_PORT = 12345;  // Порт сервера

void receiveMessages(ConnSocket& clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = clientSocket.Read(buffer, sizeof(buffer));
        if (bytesReceived < 0) {
            std::cerr << "Ошибка: соединение с сервером потеряно. Код ошибки: " << strerror(errno) << "\n";
            break;
        } else if (bytesReceived == 0) {
            std::cerr << "Соединение закрыто сервером\n";
            break;
        }
        std::cout << "Сообщение от сервера: " << buffer << std::endl;
    }
}

int main() {
    ConnSocket clientSocket;

    if (!clientSocket.ConnectToServer(SERVER_ADDRESS, SERVER_PORT)) {
        std::cerr << "Не удалось подключиться к серверу\n";
        return 1;
    }

    std::cout << "Подключено к серверу " << SERVER_ADDRESS << " на порту " << SERVER_PORT << std::endl;

    std::thread receiverThread(receiveMessages, std::ref(clientSocket));

    std::string message;
    while (true) {
        std::cout << "Введите сообщение: ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        if (!clientSocket.Write(message.c_str(), message.size())) {
            std::cerr << "Ошибка отправки сообщения\n";
            break;
        }
    }

    clientSocket.Close();
    receiverThread.join();

    return 0;
}
