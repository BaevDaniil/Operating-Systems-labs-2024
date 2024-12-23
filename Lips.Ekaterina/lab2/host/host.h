#pragma once

#include "../interface/host_interface.h"
#include "../connection/conn.h"
#include "../semaphore.h"

#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <algorithm>
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

    std::vector<std::string> reading_books;

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
    void write_to_client(const std::string& response);

    bool return_book(const std::string& book_name, const std::string& client_name);
    bool take_book(const std::string& book_name, const std::string& client_name);

    virtual bool setup_conn() = 0;

};

void read_wrap(Host& host);