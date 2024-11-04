#ifndef CONN_FIFO_H
#define CONN_FIFO_H

#include <string>

class ConnFifo {
public:
  // Конструктор и деструктор
  ConnFifo(const std::string& path, bool create);
  ~ConnFifo();

  // Запись данных в FIFO
  bool Write(const std::string& data);

  // Чтение данных из FIFO
  bool Read(std::string& data, size_t maxSize);

  // Проверка валидности FIFO
  bool IsValid() const;

private:
  std::string path; // Путь к FIFO
  int fd; // Дескриптор файла для FIFO
  bool valid; // Флаг валидности FIFO

  // Закрытие FIFO
  void Close();
};

#endif // CONN_FIFO_H
