#ifndef CONN_QUEUE_H
#define CONN_QUEUE_H

#include <string>
#include <mqueue.h>  // Заголовочный файл для работы с очередями сообщений POSIX

class ConnQueue {
public:
  // Конструктор и деструктор
  ConnQueue(const std::string& name, bool create, int maxMsgSize = 1024, int maxMsgCount = 10);
  ~ConnQueue();

  // Отправка сообщения
  bool Send(const std::string& message);

  // Получение сообщения
  bool Receive(std::string& message);

  // Проверка валидности очереди
  bool IsValid() const;

private:
  mqd_t mq;  // Дескриптор очереди сообщений
  std::string name;  // Имя очереди
  bool valid;  // Флаг валидности очереди
  int maxMsgSize;  // Максимальный размер сообщения

  // Закрытие очереди
  void Close();
};

#endif // CONN_QUEUE_H
