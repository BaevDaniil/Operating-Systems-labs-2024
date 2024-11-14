#include <iostream>
#include <string>
#include "conn_queue.hpp"

int main() {
  ConnQueue clientQueue("/queue_example", false);

  if (!clientQueue.is_valid()) {
    std::cerr << "Failed to open message queue for reading\n";
    return 1;
  }

  std::string message;
  while (true) {
    std::cout << "Enter message: ";
    std::getline(std::cin, message);

    if (message == "exit") {
      break;
    }

    if (!clientQueue.write(message)) {
      std::cerr << "Error sending message\n";
      break;
    }
  }

  return 0;
};
