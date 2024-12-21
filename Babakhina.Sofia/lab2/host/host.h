#pragma once

#include "../gui/host_gui.h"
#include "../connection/conn.h"
#include "../semaphore/semaphore.h"
#include "../book.h"

#include <QApplication>

#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/ipc.h>
#include <thread>
#include <stdlib.h>

class Host {
protected:
    pid_t pid = getpid();

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

    void read_from_client(); 
    void write_to_client(std::string response);

    virtual bool setup_conn() = 0;
};

bool return_book(std::vector<Book>& books, const std::string& book_name, std::string client_name);

bool select_book(std::vector<Book>& books, const std::string& book_name, std::string client_name);

void read_wrap(Host& host);
