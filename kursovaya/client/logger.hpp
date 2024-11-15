#ifndef _CLIENT_LOGGER_HPP
#define _CLIENT_LOGGER_HPP
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

namespace tmelfv {

class Logger {
public:
  // Конструктор по умолчанию
  Logger() = default;

  // Конструктор с начальным списком логов
  Logger(const std::vector<std::string> &initial_logs) {
    std::lock_guard<std::mutex> lock(mutex_);
    log_buffer_ = initial_logs;
  }

  // Конструктор копирования
  Logger(const Logger &other) {
    std::lock_guard<std::mutex> lock(other.mutex_);
    log_buffer_ = other.log_buffer_;
  }

  // Конструктор перемещения
  Logger(Logger &&other) noexcept {
    std::lock_guard<std::mutex> lock(other.mutex_);
    log_buffer_ = std::move(other.log_buffer_);
  }

  // Метод для добавления сообщения в лог
  void log(const std::string &message) {
    std::lock_guard<std::mutex> lock(mutex_);
    log_buffer_.push_back(message);
  }

  // Метод для вывода всех логов
  void outputLogs() const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto &log : log_buffer_) {
      std::cout << log << std::endl;
    }
  }

private:
  mutable std::mutex mutex_; // Мьютекс для защиты доступа к буферу логов
  std::vector<std::string> log_buffer_; // Вектор для хранения сообщений
};

} // namespace tmelfv
#endif // _CLIENT_LOGGER_HPP
