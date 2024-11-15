#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <cstdio>
#include <unistd.h>
#include "conn_fifo.hpp"

void read_fifo(ConnFifo& fifo, pid_t pid) {
  std::string msg;
  const size_t max_size = 1024;

  while (true) {
    if (fifo.read(msg, max_size)) {
      std::cout << ">>> " << msg << std::endl;
    }
  }
};

void write_fifo(ConnFifo& fifo, pid_t pid) {
  std::string msg;

  while (true) {
    std::getline(std::cin, msg);

    if (msg == "exit") {
      break;
    }

    if (!fifo.write(msg)) {
      std::cerr << "Error sending msg\n";
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

  std::string client_fifo_path = "/tmp/chat_client_fifo" + std::to_string(host_pid);
  std::string host_fifo_path = "/tmp/chat_host_fifo" + std::to_string(host_pid);

  ConnFifo client_fifo(client_fifo_path, false);

  if (!client_fifo.is_valid()) {
    std::cerr << "Failed to open FIFO for reading\n";
    return 1;
  }

  ConnFifo host_fifo(host_fifo_path, false);

  if (!host_fifo.is_valid()) {
    std::cerr << "Failed to open FIFO for writing\n";
    return 1;
  }

  std::thread reader(read_fifo, std::ref(client_fifo), pid);
  std::thread writer(write_fifo, std::ref(host_fifo), pid);

  reader.join();
  writer.join();

  return 0;
};
