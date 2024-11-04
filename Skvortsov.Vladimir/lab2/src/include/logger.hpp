#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

// Определение уровней логирования
enum class LogLevel {
  INFO,
  WARNING,
  ERROR,
  DEBUG
};

class Logger {
public:
  // Получение единственного экземпляра класса (синглтон)
  static Logger& getInstance() {
    static Logger instance;
    return instance;
  }

  // Установка уровня логирования
  void setLogLevel(LogLevel level) {
    logLevel = level;
  }

  // Установка файла для логирования
  void setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFile.is_open()) {
      logFile.close();
    }
    logFile.open(filename, std::ios::out | std::ios::app);
  }

  // Логирование сообщения
  void log(LogLevel level, const std::string& message) {
    if (level >= logLevel) {
      std::lock_guard<std::mutex> lock(mutex_);
      std::ostream& os = logFile.is_open() ? logFile : std::cerr;
      os << "[" << logLevelToString(level) << "] " << message << std::endl;
    }
  }

private:
  LogLevel logLevel = LogLevel::INFO;  // Уровень логирования по умолчанию
  std::ofstream logFile;         // Файловый поток для логирования
  std::mutex mutex_;           // Мьютекс для потокобезопасности

  // Приватный конструктор для синглтона
  Logger() = default;

  // Преобразование уровня логирования в строку
  std::string logLevelToString(LogLevel level) const {
    switch (level) {
      case LogLevel::INFO:  return "INFO";
      case LogLevel::WARNING: return "WARNING";
      case LogLevel::ERROR:   return "ERROR";
      case LogLevel::DEBUG:   return "DEBUG";
      default:        return "UNKNOWN";
    }
  }

  // Удаление конструктора копирования и оператора присваивания
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
};

#endif // LOGGER_H
