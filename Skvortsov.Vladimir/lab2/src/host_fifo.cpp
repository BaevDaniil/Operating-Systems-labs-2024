#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <unistd.h>
#include "conn_fifo.hpp"

void read_fifo(ConnFifo& fifo, pid_t pid) {
  std::string message;
  const size_t max_size = 1024;

  while (true) {
    if (fifo.read(message, max_size)) {
      // Extract the sender PID from the message
      size_t pos = message.find(':');
      if (pos != std::string::npos) {
        pid_t senderPid = std::stoi(message.substr(0, pos));
        std::string actualMessage = message.substr(pos + 1);

        if (senderPid != pid) {
          std::cout << ">>> " << actualMessage << std::endl;
        }
      }
    }
  }
};

void write_fifo(ConnFifo& fifo, pid_t pid) {
  std::string message;

  while (true) {
    std::getline(std::cin, message);

    if (message == "exit") {
      break;
    }

    // Prepend the host PID to the message
    std::string fullMessage = std::to_string(pid) + ":" + message;
    if (!fifo.write(fullMessage)) {
      std::cerr << "Error sending message\n";
      break;
    }
    std::cout << "<<< " << message << std::endl;
  }
};

int main() {
  pid_t pid = getpid();

  std::ofstream pid_file("host_pid.txt");
  if (!pid_file) {
    std::cerr << "Failed to open PID file for writing\n";
    return 1;
  }
  pid_file << pid;
  pid_file.close();
  std::cout << "Host PID: " << pid << std::endl;

  std::string client_fifo_path = "/tmp/chat_client_fifo" + std::to_string(pid);
  std::string host_fifo_path = "/tmp/chat_host_fifo" + std::to_string(pid);

  ConnFifo client_fifo(client_fifo_path, true);

  if (!client_fifo.is_valid()) {
    std::cerr << "Failed to open FIFO for writing\n";
    return 1;
  }

  ConnFifo host_fifo(host_fifo_path, true);

  if (!host_fifo.is_valid()) {
    std::cerr << "Failed to open FIFO for reading\n";
    return 1;
  }

  std::thread reader(read_fifo, std::ref(host_fifo), pid);
  std::thread writer(write_fifo, std::ref(client_fifo), pid);

  reader.join();
  writer.join();

  return 0;
};
