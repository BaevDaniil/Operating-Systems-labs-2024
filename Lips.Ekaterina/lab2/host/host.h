#pragma once

#include "../interface/host_interface.h"
#include "../connection/conn.h"

#include <sys/types.h>
#include <unistd.h>
#include <memory>
#include <thread>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>


class Host {
  private:
    pid_t pid = getpid();
    // pid_t clientPid = -1
    static constexpr int timeout = 5;

    std::vector<Book> books;
    

    sem_t* sem_read;
    sem_t* sem_write;

    Conn* client_conn;
    Conn* host_conn;

    bool setup_conn();

  public:
    
    bool is_running = true;

    HostWindow window;

    Host(const std::vector<Book>& books_);
    void read_from_client(); 
    void write_to_client(std::string response);
    // ~Host() 
};
