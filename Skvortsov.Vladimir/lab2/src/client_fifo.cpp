#include <iostream>
#include <string>
#include "conn_fifo.hpp"

int main() {
  ConnFifo clientFifo("/tmp/fifo_connector", false);

  if (!clientFifo.is_valid()) {
    std::cerr << "Failed to open FIFO for reading\n";
    return 1;
  }

  std::string message;
  while (true) {
    std::cout << "Enter message: ";
    std::getline(std::cin, message);

    if (message == "exit") {
      break;
    }

    if (!clientFifo.write(message)) {
      std::cerr << "Error sending message\n";
      break;
    }
  }

  return 0;
}
