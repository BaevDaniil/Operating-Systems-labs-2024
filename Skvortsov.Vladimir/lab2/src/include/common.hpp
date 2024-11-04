#ifndef COMMON_H
#define COMMON_H

#include <string>

// Общие настройки для сервера и клиента
namespace ChatConfig {
  const std::string SERVER_ADDRESS = "127.0.0.1";  // Локальный адрес сервера
  const int SERVER_PORT = 12345;           // Порт для подключения

  // Настройки очередей сообщений
  const std::string MESSAGE_QUEUE_NAME = "/chat_message_queue";
  const int MAX_MESSAGE_SIZE = 1024;
  const int MAX_MESSAGE_COUNT = 10;

  // Настройки именованных каналов (FIFO)
  const std::string FIFO_PATH = "/tmp/chat_fifo";
}

// Общие функции (если необходимы)
namespace ChatUtils {
  // Функция для обработки ошибок
  inline void logError(const std::string& message) {
    std::cerr << "Ошибка: " << message << std::endl;
  }

  // Функция для логирования информации
  inline void logInfo(const std::string& message) {
    std::cout << "Инфо: " << message << std::endl;
  }
}

#endif // COMMON_H
