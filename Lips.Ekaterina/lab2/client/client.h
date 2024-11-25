#pragma once

#include "../interface/client_interface.h"

#include <sys/types.h>
#include <memory>
#include <semaphore.h>

class Client {
private:
    pid_t host_pid;
    constexpr static int conn_timeout = 5;

public:
    bool is_running = true;

    sem_t* sem_read;
    sem_t* sem_write;

    Conn* client_conn;
    Conn* host_conn;

    Client(std::vector<Book> books);
    ClientWindow window;
    bool setup_conn();
    void read_from_host();
};