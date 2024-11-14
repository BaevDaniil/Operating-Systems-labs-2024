#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <cstdio>
#include <unistd.h>
#include "conn_queue.hpp"

void read_queue(ConnQueue& queue, pid_t pid) {
  std::string message;
  const size_t max_size = 1024;

  while (true) {
    if (queue.read(message, max_size)) {
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

void write_queue(ConnQueue& queue, pid_t pid) {
  std::string message;

  while (true) {
    std::getline(std::cin, message);

    if (message == "exit") {
      break;
    }

    // Prepend the client PID to the message
    std::string fullMessage = std::to_string(pid) + ":" + message;
    if (!queue.write(fullMessage)) {
      std::cerr << "Error sending message\n";
      break;
    }
    std::cout << "<<< " << message << std::endl;
  }
};

int main() {
  pid_t pid = getpid();

  std::ifstream pid_file("host_pid.txt");
  if (!pid_file) {
    std::cerr << "Failed to open PID file\n";
    return 1;
  }

  pid_t host_pid;
  pid_file >> host_pid;
  pid_file.close();

  if (std::remove("host_pid.txt") != 0) {
    std::cerr << "Error deleting PID file\n";
    return 1;
  }

  std::cout << "Read host PID: " << host_pid << " and removed PID file\n";

  std::string client_queue_path = "/chat_client_queue" + std::to_string(host_pid);
  std::string host_queue_path = "/chat_host_queue" + std::to_string(host_pid);

  ConnQueue client_queue(client_queue_path, false);

  if (!client_queue.is_valid()) {
    std::cerr << "Failed to open FIFO for reading\n";
    return 1;
  }

  ConnQueue host_queue(host_queue_path, false);

  if (!host_queue.is_valid()) {
    std::cerr << "Failed to open FIFO for writing\n";
    return 1;
  }

  std::thread reader(read_queue, std::ref(client_queue), pid);
  std::thread writer(write_queue, std::ref(host_queue), pid);

  reader.join();
  writer.join();

  return 0;
};
