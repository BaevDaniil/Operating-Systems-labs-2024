#pragma once

#include "../interface/client_interface.h"
#include "../semaphore.h"

#include <QMessageBox>
#include <QApplication>
#include <QObject>
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


class Client { 
protected:

    std::vector<Book> client_books;

    pid_t client_pid = getpid();
    pid_t host_pid;

public:
    
    std::string client_name = "Client";
    bool is_running = true;

    Semaphore* semaphore;

    Conn* client_conn;
    Conn* host_conn;

    ClientWindow window;

    Client(const std::vector<Book>& books);
    virtual ~Client() {}
    
    virtual bool setup_conn() = 0;
    void read_from_host();
    void write_to_host();
    void read_host_pid();

    void return_book(const std::string& book_name);
    void take_book(const std::string& book_name);
};


void read_wrap(Client& client);