#pragma once

#include "../interface/client_interface.h"

#include <sys/types.h>
#include <memory>
#include <semaphore.h>

class Client {
private:
    constexpr static int conn_timeout = 5;
    std::vector<Book> client_books;

    pid_t host_pid;

public:
    
    std::string client_name = "Linups";
    bool is_running = true;

    Conn* client_conn;
    Conn* host_conn;

    Client(std::vector<Book> books);
    ClientWindow window;
    bool setup_conn();
    void read_from_host();
    
    ~Client();
};

