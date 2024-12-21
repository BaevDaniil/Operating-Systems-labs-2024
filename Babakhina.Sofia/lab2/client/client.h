#pragma once

#include "../gui/client_gui.h"
#include "../semaphore/semaphore.h"
#include "../book.h"

#include <QMessageBox>
#include <QTimer>
#include <QApplication>

#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/ipc.h>
#include <thread>
#include <stdlib.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <csignal>


class Client{
protected:
    std::vector<Book> client_books;

    pid_t client_pid = getpid();
    pid_t host_pid;

public:
    std::string client_name = "Fryasha";
    bool is_running = true;

    Semaphore* semaphore;

    Conn* client_conn;
    Conn* host_conn;

    ClientWindow window;

    Client(std::vector<Book> books);
    virtual ~Client() {}
    
    virtual bool setup_conn() = 0;
    void read_from_host();
    void write_to_host();
    void read_host_pid();
};

void read_wrap(Client& client);
