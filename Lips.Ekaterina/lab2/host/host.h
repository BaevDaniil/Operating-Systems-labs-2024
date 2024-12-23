#pragma once

#include "../interface/host_interface.h"
#include "../connection/conn.h"
#include "../semaphore.h"


#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/ipc.h>
#include <thread>
#include <stdlib.h>



class Host {
protected:

    pid_t host_pid = getpid();
    static constexpr int timeout = 5;

    std::vector<Book> books;

    Semaphore semaphore;

    Conn* client_conn;
    Conn* host_conn;

public:
    
    bool is_running = true;

    HostWindow window;

    Host(const std::vector<Book>& books_);
    virtual ~Host() {}

    bool create_pid_file();
    void read_from_client(); 
    void write_to_client(std::string response);

    virtual bool setup_conn() = 0;

};


bool return_book(std::vector<Book>& books, const std::string& book_name, std::string client_name);

bool take_book(std::vector<Book>& books, const std::string& book_name, std::string client_name);

void read_wrap(Host& host);