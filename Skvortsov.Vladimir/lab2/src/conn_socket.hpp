#ifndef CONN_SOCKET_H
#define CONN_SOCKET_H

#include <string>
#include <netinet/in.h>

class ConnSocket {
public:
  ConnSocket();
  ~ConnSocket();

  // Создание серверного сокета
  bool CreateServerSocket(int port);

  // Принятие входящего соединения
  ConnSocket AcceptConnection();

  // Подключение к серверу (для клиента)
  bool ConnectToServer(const std::string& address, int port);

  // Отправка данных
  bool Write(const void* buffer, size_t size);

  // Получение данных
  ssize_t Read(void* buffer, size_t size);

  // Проверка валидности сокета
  bool IsValid() const;

  // Закрытие сокета
  void Close();

private:
  int sockfd;  // Дескриптор сокета
  sockaddr_in addr;  // Структура адреса сокета

  // Конструктор для создания объекта с уже существующим сокетом
  ConnSocket(int sockfd, sockaddr_in addr);
};

#endif // CONN_SOCKET_H
