#ifndef CONN_H
#define CONN_H

#include <string>

// Абстрактный базовый класс для всех типов соединений
class Conn {
public:
  virtual ~Conn() {}

  // Чтение данных
  virtual bool Read(std::string& data, size_t maxSize) = 0;

  // Запись данных
  virtual bool Write(const std::string& data) = 0;

  // Проверка валидности соединения
  virtual bool IsValid() const = 0;
};

#endif // CONN_H
