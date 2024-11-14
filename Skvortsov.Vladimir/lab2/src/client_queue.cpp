#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <cstdio>
#include <unistd.h>
#include "conn_queue.hpp"

void read_queue(ConnQueue& queue, pid_t pid) {
  std::string msg;
  const size_t max_size = 1024;

  while (true) {
    if (queue.read(msg, max_size)) {
      std::cout << ">>> " << msg << std::endl;
    }
  }
};

void write_queue(ConnQueue& queue, pid_t pid) {
  std::string msg;

  while (true) {
    std::getline(std::cin, msg);

    if (msg == "exit") {
      break;
    }

    if (!queue.write(msg)) {
      std::cerr << "Error sending message\n";
      break;
    }
    std::cout << "<<< " << msg << std::endl;
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
