#include <iostream>
#include <string>
#include "conn_fifo.hpp"

int main() {
  ConnFifo hostFifo("/tmp/fifo_example", true);

  if (!hostFifo.is_valid()) {
    std::cerr << "Failed to create FIFO for writing\n";
    return 1;
  }

  std::string message;
  while (true) {
    if (!hostFifo.read(message, 1024)) {
      std::cerr << "Error reading message\n";
      break;
    }
    std::cout << "Received message: " << message << std::endl;
  }

  return 0;
}
