#pragma once
#include "../gui/client_gui.h"
#include <sys/types.h>
#include <memory>
#include <semaphore.h>

class Client {
private:
    pid_t hostPid;
    constexpr static int connTimeout = 5;

public:
    bool is_running = true;

    sem_t* semRead;
    sem_t* semWrite;

    Conn* client_conn;
    Conn* host_conn;

    Client(std::vector<Book> books);
    ClientWindow window;
    bool setup_conn();
    void read_from_host();
};