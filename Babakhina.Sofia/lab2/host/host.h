#pragma once
#include "../gui/host_gui.h"
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
    static Host instance;
    pid_t pid = getpid();
    pid_t clientPid = -1;
    static constexpr int timeout = 5;

    sem_t* semRead;
    sem_t* semWrite;

    Conn* client_conn;
    Conn* host_conn;

    bool setup_conn();

    std::vector<Book> books;

public:
    
    bool is_running = true;

    HostWindow window;

    Host(const std::vector<Book>& books_);
    void read_from_client(); 
    void write_to_client(std::string response);

    void Terminate(int status);

    static void SignalHandler(int signum, siginfo_t *si, void *data);
};


void Host::SignalHandler(int signum, siginfo_t *si, void *data) {
    switch (signum) {
        case SIGTERM:
            break;
        case SIGUSR1:
            break;
        default:
            break;
        }
}
