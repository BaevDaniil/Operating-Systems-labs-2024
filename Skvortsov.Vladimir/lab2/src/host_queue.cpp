#include <iostream>
#include <string>
#include "conn_queue.hpp"

int main() {
  ConnQueue hostQueue("/queue_example", true);

  if (!hostQueue.is_valid()) {
    std::cerr << "Failed to create message queue for writing\n";
    return 1;
  }

  std::string message;
  const size_t max_size = 1024;

  while (true) {
    if (!hostQueue.read(message, max_size)) {
      std::cerr << "Error reading message\n";
      break;
    }
    std::cout << "Received message: " << message << std::endl;
  }

  return 0;
}
